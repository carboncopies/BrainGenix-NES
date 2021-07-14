import os
import urllib.request, urllib.error, urllib.parse
import random
import time
#import cv2  # Import OpenCV2

from brian2 import *

defaultclock.dt = 1*ms
prefs.codegen.target = 'cython'
prefs.logging.std_redirection = False
set_device('cpp_standalone', clean=True)
# set_device('cpp_standalone', clean=True, build_on_run=False)
# device.build()
filename = os.path.abspath('Megamind.avi')

#if not os.path.exists(filename):
#    print('Downloading the example video file')
#    response = urllib.request.urlopen('http://docs.opencv.org/2.4/_downloads/Megamind.avi')
#    data = response.read()
#    with open(filename, 'wb') as f:
#        f.write(data)

#video = cv2.VideoCapture(filename)
#width, height, frame_count = (int(video.get(cv2.CAP_PROP_FRAME_WIDTH)),
#                              int(video.get(cv2.CAP_PROP_FRAME_HEIGHT)),
#                              int(video.get(cv2.CAP_PROP_FRAME_COUNT)))

width, height, frame_count = 100, 100, 100

fps = 24
time_between_frames = 1*second/fps

@implementation('cpp', '''
double video_input(const int x, const int y)
{
    int r = rand();
    int denominator = std::numeric_limits<int>::max();
    return r / static_cast<double>(denominator);
    // return 42;
}
''', headers=['<cstdlib>', '<limits>'])
@check_units(x=1, y=1, result=1)
def video_input(x, y):
    # we assume this will only be called in the custom operation (and not for
    # example in a reset or synaptic statement), so we don't need to do indexing
    # but we can directly return the full result
#   _, frame = video.read()
#   grayscale = frame.mean(axis=2)
#   grayscale /= 128.  # scale everything between 0 and 2
#   return grayscale.ravel() - grayscale.ravel().mean()
    return random.random()
    # return 42


N = width * height
tau, tau_th = 10*ms, time_between_frames
G = NeuronGroup(N, '''dv/dt = (-v + I)/tau : 1
                      dv_th/dt = -v_th/tau_th : 1
                      row : integer (constant)
                      column : integer (constant)
                      I : 1 # input current''',
                threshold='v>v_th', reset='v=0; v_th = 3*v_th + 1.0',
                method='exact')
G.v_th = 1
G.row = 'i//width'
G.column = 'i%width'

G.run_regularly('I = video_input(column, row)',
                dt=time_between_frames)
# G.run_regularly('I = 42',
#                 dt=time_between_frames)
mon = SpikeMonitor(G)
runtime = frame_count*time_between_frames
t0 = time.time()
run(runtime, report='text')
# for _ in range(frame_count):
#     run(time_between_frames, report='text')
t1 = time.time()
print(f'TIME: {t1 - t0}')

# Avoid going through the whole Brian2 indexing machinery too much
i, t, row, column = mon.i[:], mon.t[:], G.row[:], G.column[:]

stepsize = 100*ms

# def other_next_spikes():
#     step = 0
#     while step * stepsize <= runtime:
#         spikes = i[(t>=step*stepsize) & (t<(step+1)*stepsize)]
#         print(type(spikes), spikes)
#         step += 1
#         yield column[spikes], row[spikes]


# for gen_out in other_next_spikes():
#     pass
#     # print([(type(x), x) for x in gen_out])
# sys.exit(0)


import matplotlib.animation as animation

# TODO: Use overlapping windows
def next_spikes():
    step = next_spikes.step
    if step*stepsize > runtime:
        next_spikes.step=0
        print('DONE')
        sys.exit(0)
    spikes = i[(t>=step*stepsize) & (t<(step+1)*stepsize)]
    next_spikes.step += 1
    yield column[spikes], row[spikes]
next_spikes.step = 0



fig, ax = plt.subplots()
dots, = ax.plot([], [], 'k.', markersize=2, alpha=.25)
ax.set_xlim(0, width)
ax.set_ylim(0, height)
ax.invert_yaxis()
def run(data):
    x, y = data
    dots.set_data(x, y)

ani = animation.FuncAnimation(fig, run, next_spikes, blit=False, repeat=True,
                              repeat_delay=1000)
plt.show()
