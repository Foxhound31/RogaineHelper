# coding=utf-8
from __future__ import with_statement, print_function
import numpy as np
import matplotlib.pyplot as pp
import matplotlib.image as mpli
import os, errno
import math
import subprocess
from pytsp import atsp_tsp, dumps_matrix


################################################################################
# Point cloud generators
################################################################################
def file_points():
    points = np.loadtxt(open('points.txt', 'rb'), delimiter=' ')
    x = [point[0] for point in points]
    y = [point[1] for point in points]
    ax = np.array(x)
    ay = np.array(y)
    xy = np.array([ax, ay])
    return xy


def main():
    img = mpli.imread('mr2014_map.png')
    pp.imshow(img)
    fig = pp.figure(1)

    def onclick(event):
        #print('button=%d, x=%d, y=%d, xdata=%f, ydata=%f'%(
        #     event.button, event.x, event.y, event.xdata, event.ydata))
        circle1 = pp.Circle((event.xdata,event.ydata), 20, color='r')
        x = event.xdata
        y = event.ydata
        # circle1 = pp.Circle((x, y), 20, color='r')
        print(int(event.xdata), int(event.ydata))
        pp.gcf().gca().add_artist(circle1)
        pp.draw()

    cid = fig.canvas.mpl_connect('button_press_event', onclick)
    pp.show()



if __name__ == '__main__':
    main()
