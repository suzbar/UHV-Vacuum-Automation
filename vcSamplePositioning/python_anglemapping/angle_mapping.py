"""
Contains all the functions necessary to map angles in output files to 360 range
/**
 * Copyright (C) <2018>  <Suzana Barreto>

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

"""

import numpy
import csv
import os

import stat_pipeline.util_functions as ufunc


def check_directions(actuator_array):
    print("direction check")
    clock_wise = False
    anti_clockwise = False
    for i in range(0,len(actuator_array)):
        if (i + 1) >= len(actuator_array):
            break
        else:
            # print(actuator_array[i])
            # print(actuator_array[i+1])
            if actuator_array[i][0] > actuator_array[i+1][0]:
                anti_clockwise = True
            else:
                clock_wise = True

    if clock_wise and anti_clockwise:
        return 3
    elif clock_wise:
        return 2
    else:
        return 1  # anticlockwise


def do_cw(angle, act_angle):
    # print("clockwise ")
    if 90 >= act_angle >= 0:
        new_angle = angle
    elif 270 >= act_angle > 90:
        new_angle = angle - 180
        new_angle *= -1
    else:
        new_angle = angle + 360

    return new_angle


def do_acw(angle, act_angle):
    # print("ant clockwise")
    if 0 >= act_angle >= -91:
        new_angle = angle
    elif -91 > act_angle >= -270:
        new_angle = angle + 180
        new_angle *= -1  # angles have now become
    else:
        new_angle = angle - 360

    return new_angle


def do_acw_with_offset(angle, act_angle, mode):
    print(" anti clockwise with offset")
    # camera angles start negative [0,-90] [-90,0] then at 180 become [0,90] [90,0]
    if 0 >= act_angle > -46:
        new_angle = angle
    elif -46 > act_angle >= -225:
        new_angle = angle + 90
    else:
        new_angle = angle - 360

    """if mode != 3:
        if new_angle > 0:
            new_angle *= -1 """

    return new_angle


def do_cw_with_offset(angle, act_angle, mode):

    if 136 > act_angle >= 0:
        new_angle = angle
    elif 315 >= act_angle >= 136:
        new_angle = angle - 270
    else:
        new_angle = angle + 360

    """if mode != 3:
        if new_angle < 0:
            new_angle *= -1"""

    return new_angle


def map_angles(cam_infile, actuator_infile, offset, mode):
    """
    reads in files and them maps the moves from an euler pi/2 system to a 360 degree system
    there are three types of files here, clockwise rotations, anticlockwise rotations and mixed moves so need a mechanism
    to detect the motion, there are also two files, one with an offset and the other without.
    :param cam_infile:
    :param actuator_infile:
    :param offset:
    :param mode: the type of file - and how to treat computations 1=acw,2=cw,3=m,4=l
    :return:
    """
    # print("AM")
    mapped_angles = []
    # read in the files
    try:
        cam_arrays = [numpy.asarray(row, dtype=float) for row in csv.reader(open(cam_infile), delimiter='\t')]
    except:
        cam_arrays = [numpy.asarray(row, dtype=float) for row in csv.reader(open(cam_infile), delimiter=',')]
    act_arrays = [numpy.asarray(row, dtype=float) for row in csv.reader(open(actuator_infile), delimiter=',')]
    # mode = check_directions(act_arrays)
    # check the lengths of arrays
    if len(cam_arrays) != len(act_arrays) and len(act_arrays) < len(cam_arrays):
        fill_array = []
        # fill actuator array with 30 frames worth of initial location data
        val_to_fill = len(cam_arrays) - len(act_arrays)

        for i in range(val_to_fill):
            print(act_arrays[0])
            temp = numpy.array(act_arrays[0])
            fill_array.append(temp)

        # concatenate the two arrays together
        copy_act = act_arrays
        # print(len(act_arrays))
        act_arrays = numpy.concatenate((fill_array, copy_act),axis=0)
    # what we have at this stage is an array[x][6] of cam poses and array [x][3] of actuator output
    # create a single column array from each with only the the z rotations
    # print(cam_arrays)
    if cam_arrays[-1].size == 0:
        cam_arrays = numpy.delete(cam_arrays, -1)

    a = cam_arrays
    # split off the y axis column
    cam_ys = numpy.array(a)[:, 1]
    # split off the z axis column
    act_zeds = numpy.array(act_arrays)[:, 0]
    # this is for mixed modes because sometimes the motor is stationary = 0, 1 is clockwise, -1 anti clockwise
    prev = 0
    if offset:
        # print("offset = true")
        # print(cam_ys)
        i = 0
        for angle in cam_ys:
            if mode == 1:  # anti clockwise
                # print(mode)
                # camera angles start negative [0,-90] [-90,0] then at 180 become [0,90] [90,0]
                new_angle = do_acw_with_offset(angle, act_zeds[i], mode)
            elif mode == 2:  # clock wise
                # print(mode)
                new_angle = do_cw_with_offset(angle, act_zeds[i], mode)
            else:
                # mixed mode - check whether the current angle is positive or negative, this determines how
                # the angles are treated
                if act_zeds[i] >= 0:
                    # treat as clockwise
                    # print(mode)
                    new_angle = do_cw_with_offset(angle, act_zeds[i], mode)
                else:
                    # treat as acw direction
                    new_angle = do_acw_with_offset(angle, act_zeds[i], mode)

            mapped_angles.append(new_angle)
            i += 1

    else:  # straight on view, no offset present
        # print("offset = False")
        # print("mode is ...")
        # print(mode)

        i = 0
        for angle in cam_ys:
            if mode == 1:  # anti - clockwise
                print(mode)
                new_angle = do_acw(angle,act_zeds[i])
            elif mode == 2:
                # clock wise
                # print(mode)
                new_angle = do_cw(angle,act_zeds[i])
            else:
                # mixed mode - check whether the current angle is positive or negative, this determines how
                # the angles are treated
                if act_zeds[i] >= 0:
                    # treat as clockwise
                    # print(mode)
                    new_angle = do_cw(angle, act_zeds[i])
                else:
                    # treat as acw direction
                    new_angle = do_acw(angle, act_zeds[i])

            mapped_angles.append(new_angle)
            i += 1

    # at this point we have a new column array replace the existing pose parameter with the new values
    # and write to file
    # print(mapped_angles)
    n = 0
    for arr in cam_arrays:
        arr[1] = mapped_angles[n]
        n += 1

    # for testing - outfilename = os.path.split(cam_infile)[0] + "/test_data.csv"
    outfilename = os.path.splitext(cam_infile)[0] + ".csv"
    # print(outfilename)
    ufunc.write_to_csv(cam_arrays,outfilename)
    return mode




# cam_infile = "/home/szb/Documents/sample_tracking/trackingExperiment/mono/test/2017-5-28_20-10/outfile3.dat"
# act_infile = "/home/szb/Documents/sample_tracking/trackingExperiment/mono/test/2017-5-28_20-10/actuatorsout.csv"

# map_angles(cam_infile,act_infile,False)
