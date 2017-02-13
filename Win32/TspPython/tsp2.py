# coding=utf-8
from __future__ import with_statement, print_function
import numpy as np
import matplotlib.pyplot as pp
import matplotlib as mpl
import matplotlib.image as mpli
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
import os, errno
import math
import subprocess
import Tkinter as Tk
import ttk
from pytsp import atsp_tsp, dumps_matrix

pointsfilename = "points.txt"
imagefilename = "mr2014_map.png"


################################################################################
# Remove file if it exists or do nothing otherwise
################################################################################
def silentremove(filename):
    try:
        os.remove(filename)
    except OSError as e:  # this would be "except OSError, e:" before Python 2.6
        if e.errno != errno.ENOENT:  # errno.ENOENT = no such file or directory
            raise  # re-raise exception if a different error occured


################################################################################
# Get points from file
################################################################################
def file_points():
    points = np.loadtxt(open(pointsfilename, 'rb'), delimiter=' ')
    x = [point[0] for point in points]
    y = [point[1] for point in points]
    ax = np.array(x)
    ay = np.array(y)
    xy = np.array([ax, ay])
    return xy


################################################################################
# Make matrix of distances between all points
################################################################################
def make_dist_matrix(x, y):
    """Creates distance matrix for the given coordinate vectors"""
    N = len(x)
    xx = np.vstack((x,) * N)
    yy = np.vstack((y,) * N)
    return np.sqrt((xx - xx.T) ** 2 + (yy - yy.T) ** 2)


################################################################################
# Call concorde symmetric TSP algorithm
################################################################################
def run_concorde(tsp_path, start=None, solver="concorde"):
    if solver.lower() == 'concorde':
        try:
            output = subprocess.check_output(['concorde.exe', tsp_path])
        except OSError as exc:
            print('OSError ' + str(exc))
            if "No such file or directory" in str(exc):
                raise TSPSolverNotFound("is not found on your path or is not executable")
        except subprocess.CalledProcessError as exc:
            print('CalledProcessError ' + exc)

        solf = os.path.splitext(os.path.basename(tsp_path))[0] + ".sol"

        with open(solf) as src:
            sol = src.read()

        raw = [int(x) for x in sol.split()[1:]]  # first is just n cities

        metadata = output.strip().split("\n")
        for line in metadata:
            if line.startswith("Optimal Solution:"):
                solution = float(line.split(":")[1])

    elif solver.lower() == 'lkh':
        LKH = os.environ.get('LKH', 'LKH')
        par_path, out_path = _create_lkh_par(tsp_path)
        try:
            output = subprocess.check_output([LKH, par_path], shell=False)
        except OSError as exc:
            if "No such file or directory" in str(exc):
                raise TSPSolverNotFound(
                    "{0} is not found on your path or is not executable".format(LKH))

        meta = []
        raw = []
        solution = None
        with open(out_path) as src:
            header = True
            for line in src:
                if header:
                    if line.startswith("COMMENT : Length ="):
                        solution = int(line.split(" ")[-1])
                    if line.startswith("TOUR_SECTION"):
                        header = False
                        continue
                    meta.append(line)
                    continue

                else:
                    if line.startswith("-1"):
                        break
                    else:
                        raw.append(int(line.strip()) - 1)  # correct to zero-indexed

        metadata = "".join(meta)

    if start:
        # rotate to the beginning of the route
        while raw[0] != start:
            raw = raw[1:] + raw[:1]

    return {'tour': raw,
            'solution': solution,
            'metadata': metadata}


################################################################################
# Calculate distamce between segment and point
################################################################################
def dist_segment_point(x1, y1, x2, y2, x3, y3):  # x3,y3 is the point
    px = x2 - x1
    py = y2 - y1

    something = px * px + py * py

    u = ((x3 - x1) * px + (y3 - y1) * py) / float(something)

    if u > 1:
        u = 1
    elif u < 0:
        u = 0

    x = x1 + u * px
    y = y1 + u * py

    dx = x - x3
    dy = y - y3

    # Note: If the actual distance does not matter,
    # if you only want to compare what this function
    # returns to other results of this function, you
    # can just return the squared distance instead
    # (i.e. remove the sqrt) to gain a little performance

    dist = math.sqrt(dx * dx + dy * dy)

    return dist


################################################################################
# Calculate route
################################################################################
def calculate_route(matrix):
    matrix_sym = atsp_tsp(matrix, strategy="avg")

    # write temporary file in the `tsp` format
    outf = "myroute_concorde.tsp"

    silentremove(outf)
    silentremove("myroute_concorde.mas")
    silentremove("myroute_concorde.pul")
    silentremove("myroute_concorde.sav")
    silentremove("myroute_concorde.sol")
    silentremove("o.txt")
    silentremove("o2.txt")
    silentremove("Omyroute_concorde.mas")
    silentremove("Omyroute_concorde.pul")
    silentremove("Omyroute_concorde.sav")

    with open(outf, 'w') as dest:
        dest.write(dumps_matrix(matrix_sym, name="My Route"))
    # print(dumps_matrix(matrix_sym, name="My Route"))
    # run the optimization with concorde
    tour = run_concorde(outf, start=0, solver="concorde")
    # or with LKH
    # tour = run(outf, start=10, solver="LKH")
    tour['tour'].append(tour['tour'][0])
    return tour

def find_nearest_point(xy, x, y):
    if x is None or y is None:
        return None

    for i in range(xy[0].size):
        if (x - xy[0][i]) * (x - xy[0][i]) + (y - xy[1][i]) * (y - xy[1][i]) <= 400:
            return i
    return None


class Mbox(object):

    root = None

    def __init__(self, root):
        tki = Tk
        self.top = tki.Toplevel(Mbox.root)
        self.root = root

        frm = tki.Frame(self.top, borderwidth=4, relief='ridge')
        frm.pack(fill='both', expand=True)

        country = ('USA', 'Canada', 'Australia')
        self.combobox = ttk.Combobox(self.top, textvariable=country)
        self.combobox['values'] = (0.1, 1, 10)
        self.combobox.current(1)
        self.combobox.pack(padx=1, pady=1)
        self.resultValue = None

        button_ok = tki.Button(self.top, text='OK', command=self._quit)
        button_ok.pack(padx=1, pady=1)

    #     b_submit = tki.Button(frm, text='Submit')
    #     b_submit['command'] = lambda: self.entry_to_dict(dict_key)
    #     b_submit.pack()
    #
    #     b_cancel = tki.Button(frm, text='Cancel')
    #     b_cancel['command'] = self.top.destroy
    #     b_cancel.pack(padx=4, pady=4)
    #
    # def entry_to_dict(self, dict_key):
    #     data = self.entry.get()
    #     if data:
    #         d, key = dict_key
    #         d[key] = data
    #         self.top.destroy()

    def result(self):
        print (self.resultValue)
        return float(self.resultValue)

    def _quit(self):
        self.resultValue = self.combobox.get()
        self.top.destroy()



################################################################################
# Class for GUI
################################################################################
class RogaineHelper:
    def __init__(self):
        self.xy = file_points()
        # matrix representing the distance between all your points of interest
        self.matrix = make_dist_matrix(self.xy[0, :], self.xy[1, :])
        self.path = None

        self.point_selected = None
        self.segment_i = None
        self.segment_j = None

        self.root = Tk.Tk()
        self.root.wm_title("RogaineHelper")

        self.fig = mpl.figure.Figure()
        self.subplot = self.fig.add_subplot(111)

        # a tk.DrawingArea
        canvas = FigureCanvasTkAgg(self.fig, master=self.root)
        canvas.show()
        canvas.get_tk_widget().pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)

        toolbar = NavigationToolbar2TkAgg(canvas, self.root)
        toolbar.update()

        self.fig.canvas.mpl_connect('button_press_event', self.onclick)
        self.fig.canvas.mpl_connect('motion_notify_event', self.onmove)

        tour = calculate_route(self.matrix)
        self.plot_route(tour)

        button = Tk.Button(master=self.root, text='Quit', command=self._quit)
        button.pack(side=Tk.BOTTOM)

        # context menu for point
        self.pmenu = Tk.Menu(self.root, tearoff=0)
        self.pmenu.add_command(label="remove", command=self.remove_point)

        # context menu for segment
        self.smenu = Tk.Menu(self.root, tearoff=0)
        self.smenu.add_command(label="set coeff", command=self.set_segment_coeff)

        # self.txt = self.fig.text(0.7, 0.9, '', transform=ax.transAxes)
        self.txt = self.fig.text(0.1, 0.9, '')

        Tk.mainloop()

    def onclick(self, event):
        if event.xdata is None or event.ydata is None:
            return
        print('button=%d, x=%d, y=%d, xdata=%f, ydata=%f' % (event.button, event.x, event.y, event.xdata, event.ydata))
        # circle1 = pp.Circle((event.x,event.y), 4, color='r')
        x = event.xdata
        y = event.ydata
        # circle1 = pp.Circle((x, y), 20, color='r')
        # pp.gcf().gca().add_artist(circle1)
        self.point_selected = None
        xy = self.xy

        self.point_selected = find_nearest_point(xy, x, y)

        if self.point_selected is not None:
            print("point {0} selected".format(str(self.point_selected)))
            if event.button == 3:
                print(self.fig.canvas.get_tk_widget().winfo_geometry())
                self.pmenu.post(int(event.x), int(event.y))
        else:
            for i in range(xy[0].size):
                for j in range(xy[0].size):
                    minx = min(xy[0][i], xy[0][j])
                    maxx = max(xy[0][i], xy[0][j])
                    miny = min(xy[1][i], xy[1][j])
                    maxy = max(xy[1][i], xy[1][j])
                    if minx <= x <= maxx and miny <= y <= maxy:
                        if dist_segment_point(xy[0][i], xy[1][i], xy[0][j], xy[1][j], x, y) <= 10:
                            path = self.path
                            for k in range(len(path)):
                                k1 = k + 1
                                if k1 == len(path):
                                    k1 = 0
                                if path[k] == i and path[k1] == j:
                                    print('segment(%d,%d)' % (i, j))
                                    self.segment_i = i
                                    self.segment_j = j
                                    if event.button == 3:
                                        self.smenu.post(int(event.x), int(event.y))

    def onmove(self, event):
        x, y = event.xdata, event.ydata
        point = find_nearest_point(self.xy, x, y)
        if point is not None:
            self.txt.set_text('point %d' % (point))
        else:
            self.txt.set_text('')

        self.fig.canvas.draw()

    def _quit(self):
        self.root.quit()  # stops mainloop
        self.root.destroy()  # this is necessary on Windows to prevent Fatal Python Error:
        # PyEval_RestoreThread: NULL tstate

    def remove_point(self):
        print("remove_point %d" % self.point_selected)
        a0 = np.delete(self.xy[0], self.point_selected)
        a1 = np.delete(self.xy[1], self.point_selected)
        self.xy = np.array([a0, a1])
        self.matrix = make_dist_matrix(self.xy[0, :], self.xy[1, :])
        tour = calculate_route(self.matrix)
        self.subplot.clear()
        self.plot_route(tour)
        self.fig.canvas.draw()

    def set_segment_coeff(self):
        i = self.segment_j
        j = self.segment_j
        xy = self.xy
        print("set_segment_coeff(%d,%d)" % (i, j))
        mbox = Mbox(self.root)
        self.root.wait_window(mbox.top)
        coeff = mbox.result()
        xx = [xy[0][i], xy[0][j]]
        yy = [xy[1][i], xy[1][j]]

        self.matrix[i][j] *= coeff
        self.matrix[j][i] *= coeff
        tour = calculate_route(self.matrix)
        self.subplot.clear()
        self.plot_route(tour)
        self.fig.canvas.draw()

    def plot_route(self, tour):
        self.path = tour['tour']
        print('tour' + str(self.path))
        coeff = 1.0 / 238
        print('solution ' + str(tour['solution'] * coeff))

        img = mpli.imread(imagefilename)
        self.subplot.imshow(img)
        self.subplot.plot(self.xy[0, self.path], self.xy[1, self.path], 'o-')


def main():
    RogaineHelper()


if __name__ == '__main__':
    main()
