# Before running this, make sure to activate the necessary venv!
# See the example in igneous_calls.sh.

from taskqueue import LocalTaskQueue
import igneous.task_creation as tc
from cloudvolume import CloudVolume
from cloudfiles import CloudFiles
from igneous.types import DownsampleMethods

import os
import argparse

Parser = argparse.ArgumentParser(description="Igneous tasks")
#Parser.add_argument("-Port", default=1335, type=int, help="Port number to connect to")
Parser.add_argument("--datapath", default=".", type=str, help="Path to Segmentation data.")
Parser.add_argument("--mip", default=0, type=int, help="Resolution hierarchy level at which to begin downsampling, 0=highest res.")
Parser.add_argument("--parallel", default=1, type=int, help="Number of parallel cores to use (default 1, 0 means all cores).")
Parser.add_argument("--fillmissing", default=False, type=bool, help=" Whether to fail (default) or fill missing image files.")
Parser.add_argument("--taskleasesec", default=60, type=int, help="Maximum seconds to lease a task (default: 60 seconds).")
Parser.add_argument("--volumetric", default=False, type=bool, help="Volumetric 2x2x2 downsampling instead of 2x2x1 (default: False).")
Args = Parser.parse_args()

if not Args.datapath:
    print('Missing data path.')
    exit(1)

print("Igneous data path: %s" % Args.datapath)
print("Igneous mip: %d" % Args.mip)
print("Igneous parallel cores: %d" % Args.parallel)
print("Task lease seconds: %d" % Args.taskleasesec)
print("Fill missing images: %s" % str(Args.fillmissing))

print('\nCleaning up...')

tq = LocalTaskQueue(parallel=1)
tasks = tc.create_mesh_deletion_tasks(
    Args.datapath, magnitude=2, mesh_dir=None
    )
tq.insert(tasks)
tq.execute()

cv = CloudVolume(Args.datapath)
cf = CloudFiles(cv.mesh.meta.layerpath, progress=True)
if not cv.mesh.meta.is_sharded():
    def remove_paths():
        for path in cf.list():
            N = len(os.path.basename(path).split(":"))
            # temp file: segid:0:fragment_name
            if N == 3:
                yield path
else:
    def remove_paths():
        for path in cf.list():
            _, ext = os.path.splitext(path)
            if ext in (".labels", ".frags"):
                yield path
cf.delete(remove_paths())

print('\nDownsampling...')

def compute_bounds(path, mip, xrange, yrange, zrange):
    bounds = None
    if xrange or yrange or zrange:
        bounds = CloudVolume(path).meta.bounds(mip)
    if xrange:
        xrange = sorted(xrange)
        bounds.minpt.x = xrange[0]
        bounds.maxpt.x = xrange[1]
    if yrange:
        yrange = sorted(yrange)
        bounds.minpt.y = yrange[0]
        bounds.maxpt.y = yrange[1]
    if zrange:
        zrange = sorted(zrange)
        bounds.minpt.z = zrange[0]
        bounds.maxpt.z = zrange[1]
    return bounds

if Args.volumetric:
    factor = (2,2,2)
else:
    factor = (2,2,1)
bounds = compute_bounds(Args.datapath, Args.mip, None, None, None)

tq = LocalTaskQueue(parallel=Args.parallel)
tasks = tc.create_downsampling_tasks(
    Args.datapath, mip=Args.mip, fill_missing=Args.fillmissing, 
    num_mips=None, sparse=False, 
    chunk_size=None, encoding=None,
    delete_black_uploads=False, 
    background_color=0, 
    compress=None,
    factor=factor, bounds=bounds,
    bounds_mip=Args.mip,
    memory_target=int(3.5e9),
    encoding_level=None,
    method=DownsampleMethods.AUTO,
    encoding_effort=5,
    )
tq.insert(tasks)
tq.execute()

print("\nForging meshes...")
# igneous mesh forge $fillarg $datapath --queue ./mf-queue --mip 0

tq = LocalTaskQueue(parallel=Args.parallel)
tasks = tc.create_meshing_tasks(
    Args.datapath, mip=Args.mip, shape=(448, 448, 448), 
    simplification=True, max_simplification_error=40,
    mesh_dir=None, cdn_cache=False, dust_threshold=None,
    object_ids=None, progress=False, fill_missing=Args.fillmissing,
    encoding='precomputed', spatial_index=True, 
    sharded=False, compress='gzip', closed_dataset_edges=True,
    dust_global=False, fill_holes=0,
    )
tq.insert(tasks)
tq.execute()

print("\nMerging meshes...")
# igneous mesh merge $datapath --queue ./mm-queue
tasks = tc.create_mesh_manifest_tasks(
    Args.datapath, magnitude=2, mesh_dir=None
    )
tq.insert(tasks)
tq.execute()

print("Igneous Done!")
