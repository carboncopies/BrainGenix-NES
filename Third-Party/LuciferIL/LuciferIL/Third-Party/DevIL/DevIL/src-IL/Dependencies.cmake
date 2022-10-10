# # LibPNG
# message("Setting Up LibPNG For DevIL")

# # include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Third-Party/LibPNG)
# # set(PNG_DEP_FILES
# #     Third-Party/LibPNG/png.c
# #     Third-Party/LibPNG/pngerror.c
# #     Third-Party/LibPNG/pngget.c 
# #     Third-Party/LibPNG/pngmem.c 
# #     Third-Party/LibPNG/pngpread.c 
# #     Third-Party/LibPNG/pngread.c 
# #     Third-Party/LibPNG/pngrio.c 
# #     Third-Party/LibPNG/pngrtran.c 
# #     Third-Party/LibPNG/pngrutil.c
# #     Third-Party/LibPNG/pngset.c
# #     Third-Party/LibPNG/pngtrans.c
# #     Third-Party/LibPNG/pngwio.c
# #     Third-Party/LibPNG/pngwrite.c
# #     Third-Party/LibPNG/pngwtran.c
# #     Third-Party/LibPNG/pngwutil.c
# # )
# # add_library(PNG_DEPS SHARED ${PNG_DEP_FILES})
# # target_link_libraries(IL PNG_DEPS)
# set(PNG_FOUND 1)

# message("Completed Setting Up LibPNG For DevIL")


# # LibTiff

# message("Setting Up LibTIFF For DevIL")

# # include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Third-Party/LibTIFF4)
# # set(TIFF_DEP_FILES
# #     Third-Party/LibTIFF4/tif_aux.c
# #     Third-Party/LibTIFF4/tif_close.c
# #     Third-Party/LibTIFF4/tif_codec.c
# #     Third-Party/LibTIFF4/tif_color.c
# #     Third-Party/LibTIFF4/tif_compress.c
# #     Third-Party/LibTIFF4/tif_dir.c
# #     Third-Party/LibTIFF4/tif_dirinfo.c
# #     Third-Party/LibTIFF4/tif_dirread.c
# #     Third-Party/LibTIFF4/tif_dirwrite.c
# #     Third-Party/LibTIFF4/tif_dumpmode.c
# #     Third-Party/LibTIFF4/tif_error.c
# #     Third-Party/LibTIFF4/tif_extension.c
# #     Third-Party/LibTIFF4/tif_fax3.c
# #     Third-Party/LibTIFF4/tif_fax3sm.c
# #     Third-Party/LibTIFF4/tif_flush.c
# #     Third-Party/LibTIFF4/tif_getimage.c
# #     Third-Party/LibTIFF4/tif_ojpeg.c
# #     Third-Party/LibTIFF4/tif_jpeg.c
# #     Third-Party/LibTIFF4/tif_luv.c
# #     Third-Party/LibTIFF4/tif_lzma.c
# #     Third-Party/LibTIFF4/tif_lzw.c
# #     Third-Party/LibTIFF4/tif_next.c
# #     Third-Party/LibTIFF4/tif_ojpeg.c
# #     Third-Party/LibTIFF4/tif_open.c
# #     Third-Party/LibTIFF4/tif_packbits.c
# #     Third-Party/LibTIFF4/tif_pixarlog.c
# #     Third-Party/LibTIFF4/tif_predict.c
# #     Third-Party/LibTIFF4/tif_print.c
# #     Third-Party/LibTIFF4/tif_read.c
# #     Third-Party/LibTIFF4/tif_strip.c
# #     Third-Party/LibTIFF4/tif_swab.c
# #     Third-Party/LibTIFF4/tif_thunder.c
# #     Third-Party/LibTIFF4/tif_tile.c
# #     Third-Party/LibTIFF4/tif_version.c
# #     Third-Party/LibTIFF4/tif_warning.c
# #     Third-Party/LibTIFF4/tif_write.c
# #     Third-Party/LibTIFF4/tif_zip.c
# # )
# # add_library(TIFF_DEPS SHARED ${TIFF_DEP_FILES})
# # target_link_libraries(IL TIFF_DEPS)
# set(TIFF_FOUND 1)

# message("Completed Setting Up LibTIFF For DevIL")


# set(JPEG_FOUND 1)

# # copy the directores from Third-Party/Makefile.srcs
# include_directories(Third-Party/LibJPEG Third-Party/LibPNG Third-Party/LibTIFF4 Third-Party/ZLib Third-Party/LibOpenJPEG Third-Party/OpenEXR Third-Party/OpenEXR/Half Third-Party/OpenEXR/Iex Third-Party/OpenEXR/IlmImf Third-Party/OpenEXR/IlmThread Third-Party/OpenEXR/Imath Third-Party/OpenEXR/IexMath Third-Party/LibRawLite Third-Party/LibRawLite/dcraw Third-Party/LibRawLite/internal Third-Party/LibRawLite/libraw Third-Party/LibRawLite/src Third-Party/LibWebP Third-Party/LibJXR Third-Party/LibJXR/common/include Third-Party/LibJXR/image/sys Third-Party/LibJXR/jxrgluelib)
# # copy the path of Third-Party files from Third-Party/Makefile.srcs
# set(DepFiles
# Third-Party/LibJPEG/jaricom.c
# Third-Party/LibJPEG/jcapimin.c
# Third-Party/LibJPEG/jcapistd.c
# Third-Party/LibJPEG/jcarith.c
# Third-Party/LibJPEG/jccoefct.c
# Third-Party/LibJPEG/jccolor.c
# Third-Party/LibJPEG/jcdctmgr.c
# Third-Party/LibJPEG/jchuff.c
# Third-Party/LibJPEG/jcinit.c
# Third-Party/LibJPEG/jcmainct.c
# Third-Party/LibJPEG/jcmarker.c
# Third-Party/LibJPEG/jcmaster.c
# Third-Party/LibJPEG/jcomapi.c
# Third-Party/LibJPEG/jcparam.c
# Third-Party/LibJPEG/jcprepct.c
# Third-Party/LibJPEG/jcsample.c
# Third-Party/LibJPEG/jctrans.c
# Third-Party/LibJPEG/jdapimin.c
# Third-Party/LibJPEG/jdapistd.c
# Third-Party/LibJPEG/jdarith.c
# Third-Party/LibJPEG/jdatadst.c
# Third-Party/LibJPEG/jdatasrc.c
# Third-Party/LibJPEG/jdcoefct.c
# Third-Party/LibJPEG/jdcolor.c
# Third-Party/LibJPEG/jddctmgr.c
# Third-Party/LibJPEG/jdhuff.c
# Third-Party/LibJPEG/jdinput.c
# Third-Party/LibJPEG/jdmainct.c
# Third-Party/LibJPEG/jdmarker.c
# Third-Party/LibJPEG/jdmaster.c
# Third-Party/LibJPEG/jdmerge.c
# Third-Party/LibJPEG/jdpostct.c
# Third-Party/LibJPEG/jdsample.c
# Third-Party/LibJPEG/jdtrans.c
# Third-Party/LibJPEG/jerror.c
# Third-Party/LibJPEG/jfdctflt.c
# Third-Party/LibJPEG/jfdctfst.c
# Third-Party/LibJPEG/jfdctint.c
# Third-Party/LibJPEG/jidctflt.c
# Third-Party/LibJPEG/jidctfst.c
# Third-Party/LibJPEG/jidctint.c
# Third-Party/LibJPEG/jmemmgr.c
# Third-Party/LibJPEG/jmemnobs.c
# Third-Party/LibJPEG/jquant1.c
# Third-Party/LibJPEG/jquant2.c
# Third-Party/LibJPEG/jutils.c
# Third-Party/LibJPEG/transupp.c
# Third-Party/LibPNG/png.c
# Third-Party/LibPNG/pngerror.c
# Third-Party/LibPNG/pngget.c
# Third-Party/LibPNG/pngmem.c
# Third-Party/LibPNG/pngpread.c
# Third-Party/LibPNG/pngread.c
# Third-Party/LibPNG/pngrio.c
# Third-Party/LibPNG/pngrtran.c
# Third-Party/LibPNG/pngrutil.c
# Third-Party/LibPNG/pngset.c
# Third-Party/LibPNG/pngtrans.c
# Third-Party/LibPNG/pngwio.c
# Third-Party/LibPNG/pngwrite.c
# Third-Party/LibPNG/pngwtran.c
# Third-Party/LibPNG/pngwutil.c
# Third-Party/LibTIFF4/tif_aux.c
# Third-Party/LibTIFF4/tif_close.c
# Third-Party/LibTIFF4/tif_codec.c
# Third-Party/LibTIFF4/tif_color.c
# Third-Party/LibTIFF4/tif_compress.c
# Third-Party/LibTIFF4/tif_dir.c
# Third-Party/LibTIFF4/tif_dirinfo.c
# Third-Party/LibTIFF4/tif_dirread.c
# Third-Party/LibTIFF4/tif_dirwrite.c
# Third-Party/LibTIFF4/tif_dumpmode.c
# Third-Party/LibTIFF4/tif_error.c
# Third-Party/LibTIFF4/tif_extension.c
# Third-Party/LibTIFF4/tif_fax3.c
# Third-Party/LibTIFF4/tif_fax3sm.c
# Third-Party/LibTIFF4/tif_flush.c
# Third-Party/LibTIFF4/tif_getimage.c
# Third-Party/LibTIFF4/tif_jpeg.c
# Third-Party/LibTIFF4/tif_luv.c
# Third-Party/LibTIFF4/tif_lzma.c
# Third-Party/LibTIFF4/tif_lzw.c
# Third-Party/LibTIFF4/tif_next.c
# Third-Party/LibTIFF4/tif_ojpeg.c
# Third-Party/LibTIFF4/tif_open.c
# Third-Party/LibTIFF4/tif_packbits.c
# Third-Party/LibTIFF4/tif_pixarlog.c
# Third-Party/LibTIFF4/tif_predict.c
# Third-Party/LibTIFF4/tif_print.c
# Third-Party/LibTIFF4/tif_read.c
# Third-Party/LibTIFF4/tif_strip.c
# Third-Party/LibTIFF4/tif_swab.c
# Third-Party/LibTIFF4/tif_thunder.c
# Third-Party/LibTIFF4/tif_tile.c
# Third-Party/LibTIFF4/tif_version.c
# Third-Party/LibTIFF4/tif_warning.c
# Third-Party/LibTIFF4/tif_write.c
# Third-Party/LibTIFF4/tif_zip.c
# Third-Party/ZLib/adler32.c
# Third-Party/ZLib/compress.c
# Third-Party/ZLib/crc32.c
# Third-Party/ZLib/deflate.c
# Third-Party/ZLib/gzclose.c
# Third-Party/ZLib/gzlib.c
# Third-Party/ZLib/gzread.c
# Third-Party/ZLib/gzwrite.c
# Third-Party/ZLib/infback.c
# Third-Party/ZLib/inffast.c
# Third-Party/ZLib/inflate.c
# Third-Party/ZLib/inftrees.c
# Third-Party/ZLib/trees.c
# Third-Party/ZLib/uncompr.c
# Third-Party/ZLib/zutil.c
# Third-Party/LibOpenJPEG/bio.c
# Third-Party/LibOpenJPEG/cio.c
# Third-Party/LibOpenJPEG/dwt.c
# Third-Party/LibOpenJPEG/event.c
# Third-Party/LibOpenJPEG/function_list.c
# Third-Party/LibOpenJPEG/image.c
# Third-Party/LibOpenJPEG/invert.c
# Third-Party/LibOpenJPEG/j2k.c
# Third-Party/LibOpenJPEG/jp2.c
# Third-Party/LibOpenJPEG/mct.c
# Third-Party/LibOpenJPEG/mqc.c
# Third-Party/LibOpenJPEG/openjpeg.c
# Third-Party/LibOpenJPEG/opj_clock.c
# Third-Party/LibOpenJPEG/pi.c
# Third-Party/LibOpenJPEG/raw.c
# Third-Party/LibOpenJPEG/t1.c
# Third-Party/LibOpenJPEG/t2.c
# Third-Party/LibOpenJPEG/tcd.c
# Third-Party/LibOpenJPEG/tgt.c
# Third-Party/LibRawLite/internal/dcraw_common.cpp
# Third-Party/LibRawLite/internal/dcraw_fileio.cpp
# Third-Party/LibRawLite/internal/demosaic_packs.cpp
# Third-Party/LibRawLite/src/libraw_c_api.cpp
# Third-Party/LibRawLite/src/libraw_cxx.cpp
# Third-Party/LibRawLite/src/libraw_datastream.cpp
# Third-Party/LibWebP/src/dec/alpha_dec.c
# Third-Party/LibWebP/src/dec/buffer_dec.c
# Third-Party/LibWebP/src/dec/frame_dec.c
# Third-Party/LibWebP/src/dec/idec_dec.c
# Third-Party/LibWebP/src/dec/io_dec.c
# Third-Party/LibWebP/src/dec/quant_dec.c
# Third-Party/LibWebP/src/dec/tree_dec.c
# Third-Party/LibWebP/src/dec/vp8l_dec.c
# Third-Party/LibWebP/src/dec/vp8_dec.c
# Third-Party/LibWebP/src/dec/webp_dec.c
# Third-Party/LibWebP/src/demux/anim_decode.c
# Third-Party/LibWebP/src/demux/demux.c
# Third-Party/LibWebP/src/dsp/alpha_processing.c
# Third-Party/LibWebP/src/dsp/alpha_processing_mips_dsp_r2.c
# Third-Party/LibWebP/src/dsp/alpha_processing_neon.c
# Third-Party/LibWebP/src/dsp/alpha_processing_sse2.c
# Third-Party/LibWebP/src/dsp/alpha_processing_sse41.c
# Third-Party/LibWebP/src/dsp/cost.c
# Third-Party/LibWebP/src/dsp/cost_mips32.c
# Third-Party/LibWebP/src/dsp/cost_mips_dsp_r2.c
# Third-Party/LibWebP/src/dsp/cost_sse2.c
# Third-Party/LibWebP/src/dsp/cpu.c
# Third-Party/LibWebP/src/dsp/dec.c
# Third-Party/LibWebP/src/dsp/dec_clip_tables.c
# Third-Party/LibWebP/src/dsp/dec_mips32.c
# Third-Party/LibWebP/src/dsp/dec_mips_dsp_r2.c
# Third-Party/LibWebP/src/dsp/dec_msa.c
# Third-Party/LibWebP/src/dsp/dec_neon.c
# Third-Party/LibWebP/src/dsp/dec_sse2.c
# Third-Party/LibWebP/src/dsp/dec_sse41.c
# Third-Party/LibWebP/src/dsp/enc.c
# Third-Party/LibWebP/src/dsp/enc_avx2.c
# Third-Party/LibWebP/src/dsp/enc_mips32.c
# Third-Party/LibWebP/src/dsp/enc_mips_dsp_r2.c
# Third-Party/LibWebP/src/dsp/enc_msa.c
# Third-Party/LibWebP/src/dsp/enc_neon.c
# Third-Party/LibWebP/src/dsp/enc_sse2.c
# Third-Party/LibWebP/src/dsp/enc_sse41.c
# Third-Party/LibWebP/src/dsp/filters.c
# Third-Party/LibWebP/src/dsp/filters_mips_dsp_r2.c
# Third-Party/LibWebP/src/dsp/filters_msa.c
# Third-Party/LibWebP/src/dsp/filters_neon.c
# Third-Party/LibWebP/src/dsp/filters_sse2.c
# Third-Party/LibWebP/src/dsp/lossless.c
# Third-Party/LibWebP/src/dsp/lossless_enc.c
# Third-Party/LibWebP/src/dsp/lossless_enc_mips32.c
# Third-Party/LibWebP/src/dsp/lossless_enc_mips_dsp_r2.c
# Third-Party/LibWebP/src/dsp/lossless_enc_msa.c
# Third-Party/LibWebP/src/dsp/lossless_enc_neon.c
# Third-Party/LibWebP/src/dsp/lossless_enc_sse2.c
# Third-Party/LibWebP/src/dsp/lossless_enc_sse41.c
# Third-Party/LibWebP/src/dsp/lossless_mips_dsp_r2.c
# Third-Party/LibWebP/src/dsp/lossless_msa.c
# Third-Party/LibWebP/src/dsp/lossless_neon.c
# Third-Party/LibWebP/src/dsp/lossless_sse2.c
# Third-Party/LibWebP/src/dsp/rescaler.c
# Third-Party/LibWebP/src/dsp/rescaler_mips32.c
# Third-Party/LibWebP/src/dsp/rescaler_mips_dsp_r2.c
# Third-Party/LibWebP/src/dsp/rescaler_msa.c
# Third-Party/LibWebP/src/dsp/rescaler_neon.c
# Third-Party/LibWebP/src/dsp/rescaler_sse2.c
# Third-Party/LibWebP/src/dsp/ssim.c
# Third-Party/LibWebP/src/dsp/ssim_sse2.c
# Third-Party/LibWebP/src/dsp/upsampling.c
# Third-Party/LibWebP/src/dsp/upsampling_mips_dsp_r2.c
# Third-Party/LibWebP/src/dsp/upsampling_msa.c
# Third-Party/LibWebP/src/dsp/upsampling_neon.c
# Third-Party/LibWebP/src/dsp/upsampling_sse2.c
# Third-Party/LibWebP/src/dsp/upsampling_sse41.c
# Third-Party/LibWebP/src/dsp/yuv.c
# Third-Party/LibWebP/src/dsp/yuv_mips32.c
# Third-Party/LibWebP/src/dsp/yuv_mips_dsp_r2.c
# Third-Party/LibWebP/src/dsp/yuv_neon.c
# Third-Party/LibWebP/src/dsp/yuv_sse2.c
# Third-Party/LibWebP/src/dsp/yuv_sse41.c
# Third-Party/LibWebP/src/enc/alpha_enc.c
# Third-Party/LibWebP/src/enc/analysis_enc.c
# Third-Party/LibWebP/src/enc/backward_references_cost_enc.c
# Third-Party/LibWebP/src/enc/backward_references_enc.c
# Third-Party/LibWebP/src/enc/config_enc.c
# Third-Party/LibWebP/src/enc/cost_enc.c
# Third-Party/LibWebP/src/enc/filter_enc.c
# Third-Party/LibWebP/src/enc/frame_enc.c
# Third-Party/LibWebP/src/enc/histogram_enc.c
# Third-Party/LibWebP/src/enc/iterator_enc.c
# Third-Party/LibWebP/src/enc/near_lossless_enc.c
# Third-Party/LibWebP/src/enc/picture_csp_enc.c
# Third-Party/LibWebP/src/enc/picture_enc.c
# Third-Party/LibWebP/src/enc/picture_psnr_enc.c
# Third-Party/LibWebP/src/enc/picture_rescale_enc.c
# Third-Party/LibWebP/src/enc/picture_tools_enc.c
# Third-Party/LibWebP/src/enc/predictor_enc.c
# Third-Party/LibWebP/src/enc/quant_enc.c
# Third-Party/LibWebP/src/enc/syntax_enc.c
# Third-Party/LibWebP/src/enc/token_enc.c
# Third-Party/LibWebP/src/enc/tree_enc.c
# Third-Party/LibWebP/src/enc/vp8l_enc.c
# Third-Party/LibWebP/src/enc/webp_enc.c
# Third-Party/LibWebP/src/mux/anim_encode.c
# Third-Party/LibWebP/src/mux/muxedit.c
# Third-Party/LibWebP/src/mux/muxinternal.c
# Third-Party/LibWebP/src/mux/muxread.c
# Third-Party/LibWebP/src/utils/bit_reader_utils.c
# Third-Party/LibWebP/src/utils/bit_writer_utils.c
# Third-Party/LibWebP/src/utils/color_cache_utils.c
# Third-Party/LibWebP/src/utils/filters_utils.c
# Third-Party/LibWebP/src/utils/huffman_encode_utils.c
# Third-Party/LibWebP/src/utils/huffman_utils.c
# Third-Party/LibWebP/src/utils/quant_levels_dec_utils.c
# Third-Party/LibWebP/src/utils/quant_levels_utils.c
# Third-Party/LibWebP/src/utils/random_utils.c
# Third-Party/LibWebP/src/utils/rescaler_utils.c
# Third-Party/LibWebP/src/utils/thread_utils.c
# Third-Party/LibWebP/src/utils/utils.c
# # Third-Party/LibJXR/image/decode/decode.c
# # Third-Party/LibJXR/image/decode/JXRTranscode.c
# # Third-Party/LibJXR/image/decode/postprocess.c
# # Third-Party/LibJXR/image/decode/segdec.c
# # Third-Party/LibJXR/image/decode/strdec.c
# # Third-Party/LibJXR/image/decode/strdec_x86.c
# # Third-Party/LibJXR/image/decode/strInvTransform.c
# # Third-Party/LibJXR/image/decode/strPredQuantDec.c
# # Third-Party/LibJXR/image/encode/encode.c
# # Third-Party/LibJXR/image/encode/segenc.c
# # Third-Party/LibJXR/image/encode/strenc.c
# # Third-Party/LibJXR/image/encode/strenc_x86.c
# # Third-Party/LibJXR/image/encode/strFwdTransform.c
# # Third-Party/LibJXR/image/encode/strPredQuantEnc.c
# # Third-Party/LibJXR/image/sys/adapthuff.c
# # Third-Party/LibJXR/image/sys/image.c
# # Third-Party/LibJXR/image/sys/strcodec.c
# # Third-Party/LibJXR/image/sys/strPredQuant.c
# # Third-Party/LibJXR/image/sys/strTransform.c
# # Third-Party/LibJXR/jxrgluelib/JXRGlue.c
# # Third-Party/LibJXR/jxrgluelib/JXRGlueJxr.c
# # Third-Party/LibJXR/jxrgluelib/JXRGluePFC.c
# # Third-Party/LibJXR/jxrgluelib/JXRMeta.c
# )
# add_library(Deps SHARED ${DepFiles})
# target_link_libraries(IL Deps)