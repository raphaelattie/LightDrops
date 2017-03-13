#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""
Script testing the lucky imaging. Assumes already co-aligned images

@author: Raphael Attie
"""

import os
import sys
import time
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import glob
import Lucky_Imaging.ra_lucky_imaging as li


# Disable interactive mode so figure goes directory to a file and does not show up on screen
plt.ioff()

# Get the list of files
#data_dir    = '/Volumes/SDobo-A/Raphael/USET/H_Alpha/UPH20161215_short_exp/calibrated_level1.1/'
data_dir    = '/Users/rattie/Data/USET/H_Alpha/UPH20161215_short_exp/calibrated_level1.1/'
files       = glob.glob(os.path.join(data_dir, '*.fits'))
outdir      = os.path.join(data_dir, 'lucky')
outdir_jpeg = os.path.join(outdir , 'jpeg')
# Check if output directory exists. Create if not.
if not os.path.isdir(outdir):
    os.makedirs(outdir)
if not os.path.isdir(outdir_jpeg):
    os.makedirs(outdir_jpeg)

print_preview_fullsun = True

# Number of images to consider in the list of files. Total lucky images = nImages/interval
nImages = 20

## Define parameters for the block processing
# Image binning
# Binning for the quality matrix. Do not change for USET unless you know what you are doing.
binning         = 1
# Buffer space: extra marging around a reference block when aligning a template block
# bufferSpace   = 8
# Block size and binned block size
blk_size        = 32
# Quality metric
qmetric = 'entropy'

# nb of images between each final processed image = nb of images that get sorted in the quality matrix
intervals        = [20, 60]
# Number of best images to keep in the block processing.
nbest           = 5

# for interval in intervals:
#
#     blend_mode = 'aavg'
#     li.lucky_imaging_wrapper(files, outdir, outdir_jpeg, nImages,
#                             interval, nbest, binning, blk_size, blend_mode,
#                             print_preview_fullsun)
    # blend_mode = 'gblend'
    # li.lucky_imaging_wrapper(files, outdir, outdir_jpeg, nImages,
    #                          interval, nbest, binning, blk_size, blend_mode,
    #                          print_preview_fullsun)

# Change the number of best images

# nbest           = 10
# for interval in intervals:
#
#     blend_mode = 'aavg'
#     li.lucky_imaging_wrapper(files, outdir, outdir_jpeg, nImages,
#                             interval, nbest, binning, blk_size, blend_mode,
#                             print_preview_fullsun)
#     blend_mode = 'gblend'
#     li.lucky_imaging_wrapper(files, outdir, outdir_jpeg, nImages,
#                              interval, nbest, binning, blk_size, blend_mode,
#                              print_preview_fullsun)

blend_mode = 'aavg'
interval = 40

time1 = time.time()
shifts = li.lucky_imaging_wrapper(files, outdir, outdir_jpeg, nImages,
                                  interval, nbest, binning, blk_size, qmetric, blend_mode,
                                  print_preview_fullsun)
time2 = time.time()
print('Elapsed time %0.1f s' % (time2-time1))


print('done')
