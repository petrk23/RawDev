##------------------------------------##
## Make HSV tables from EXIF of a DNG ##
##------------------------------------##

import argparse
import subprocess
import os
import datetime

## Header string
header = '''/*
 * This file is part of RawDev;
 * see <https://github.com/petrk23/RawDev>.
 *
 * Copyright (C) <year> <name of author>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Cam{cam}.hpp"

const HSVMap::HSV64Scale Cam{cam}::{vname}[] = {{
'''

## Generated source indent
indent = '    ' ## 4 spaces

## Mapping function HSV cube
def index(h, huedir, s, satdir, v, valdir, x):
    return 3 + 3* v * huedir * satdir + 3*h * satdir + 3 * s + x

## Get value
def getval(f_list, h, huedir, s, satdir, v, valdir, i):
    return f_list[index(h, huedir, s, satdir, v, valdir, i)].decode('utf-8')

## Process HSV map item
def processItem(f, f_list, h, huedir, s, satdir, v, valdir):
    value = [getval(f_list, h, huedir, s, satdir, v, valdir, 0),
        getval(f_list, h, huedir, s, satdir, v, valdir, 1),
        getval(f_list, h, huedir, s, satdir, v, valdir, 2)]
    if value[0][0] == '-':
        f.write(f'{indent}{{{value[0]}, ')
    else:
        f.write(f'{indent}{{ {value[0]}, ')
    f.write(f'{value[1]}, ')
    f.write(f'{value[2]}}},\n')

## Process HSV Map
def processHSV(f, f_list, huedir, satdir, valdir):
    item = 0
    for v in range(0, valdir):
        for h in range(0, huedir):
            f.write (f'\n{indent}// val={v}, hue={h}, sat=0-{satdir - 1}\n')
            for s in range(0, satdir):
                processItem(f, f_list, h, huedir, s, satdir, v, valdir)
    f.write('};\n')

## Processing
def process(data, outName, cam, vname, comment):
    f_list = data.split() ## Get data value array
    if len(f_list) <= 16:
        print('skipped')
        return;

    # Load dimensions
    huedir = int(f_list[0])
    satdir = int(f_list[1])
    valdir = int(f_list[2])

    # File comment
    with open(outName, 'w') as f:
        f.write(header.format(cam=cam, vname=vname))
        processHSV(f, f_list, huedir, satdir, valdir)
    print('done')

################################################################################

parser = argparse.ArgumentParser(
    description='Generate files for support cameras in RawDev.')
parser.add_argument('imagePath',
    metavar='imagePath', help='Path to a dng file for input')
parser.add_argument('-c', '--cam', nargs=1, required=True,
    help='Camera model code without Canon and/or EOS.')
parser.add_argument('-x', action='store_true', default=False,
    help='Write data from exiftool to a file for future check.')
args = parser.parse_args()

## Check the input filefile name
if os.path.splitext(args.imagePath)[1] != '.dng':
    print('Input file must be a DNG digital negative. END.')
    exit(-1)

## Extract data from DNG
print('Running exiftool and getting the data ... ', flush=True, end='')
exdata = subprocess.check_output(['exiftool', '-b', '-m', '-p',
    "${ProfileHueSatMapDims} ${ProfileHueSatMapData1}$/"
    "${ProfileHueSatMapDims} ${ProfileHueSatMapData2}$/"
    '${ProfileLookTableDims} ${ProfileLookTableData}', args.imagePath])
print('done')

## Store the data for future check
if args.x == True:
    print('Saving exif data to \'exdata.txt\' ... ', flush=True, end='')
    with open('exdata.txt', 'wb') as f:
        f.write(exdata)
    print('done')

## Separate data
exdata = exdata.splitlines();

## Process HSV Map 1
cam = args.cam[0]
print('Processing HSVMap1...', flush=True, end='')
process(exdata[0], f'Cam{cam}-hsv1.cpp',
    cam, 'k_hsvMapData1', 'HSV Map Data for first illuminant')

## Process HSV Map 2
print('Processing HSVMap2...', flush=True, end='')
process(exdata[1], f'Cam{cam}-hsv2.cpp',
    cam, 'k_hsvMapData2', 'HSV Map Data for second illuminant')

print('Processing ProfileLook ... ', flush=True, end='')
process(exdata[2], f'Cam{cam}-prlook.cpp',
    cam, 'k_profileLookTableData', 'HSV Map Data for default profile look')

print('DONE!') ## Finished
