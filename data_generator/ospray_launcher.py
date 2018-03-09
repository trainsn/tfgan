#from subprocess import call
import argparse
import os
import numpy as np
from tqdm import tqdm
import math
import pdb
#from multiprocessing import Process

def sphere_dist(elevation1, azimuth1, elevation2, azimuth2):
    #pdb.set_trace()
    elevation1 = (elevation1-90) * math.pi / 180
    elevation2 = (elevation2-90) * math.pi / 180
    azimuth1 = azimuth1 * math.pi / 180
    azimuth2 = azimuth2 * math.pi / 180
    return math.acos(math.cos(elevation1) * math.cos(elevation2) * math.cos(azimuth1-azimuth2) + math.sin(elevation1) * math.sin(elevation2))   


parser = argparse.ArgumentParser("python ospray_launcher.py")
#parser.add_argument("ospray_renderer", help="path to ospray executable")
#parser.add_argument("vti_path", help="path to vti file")
#parser.add_argument("view", help="path to npy version view file")
#parser.add_argument("opacity", help="path to npy version opacity file")
#parser.add_argument("color", help="path to npy version color file")

parser.add_argument("outdir", help="output directory")
#parser.add_argument("-v", "--var", default="1.5",
#                    help="variance threshold DEFAULT=1.5")
#parser.add_argument("-r", "--rounds", default="1",
#                    help="number of rounds DEFAULT=1")
parser.add_argument("-s", "--start", default=0, type=int,
                    help="starting index DEFAULT=0")
parser.add_argument("-e", "--end", default=100, type=int,
                    help="ending index DEFAULT=100")
#parser.add_argument("-p", "--procs", default=1,
#                    type=int, help="number of procs")
#parser.add_argument("-u", "--up", default=[-1.0,0.0,0.0],
#                    nargs='*', help="up vector")
#parser.add_argument("-i", "--index", action="store_true",
#                    help="indexing after image generation")
args = parser.parse_args()
print(args)


# convert npy to txt input
if args.outdir[-1] != "/":
    args.outdir += "/"
if not os.path.exists(args.outdir):
    os.makedirs(args.outdir)
#print(args.outdir)

params_dir = args.outdir + "params/"
if not os.path.exists(params_dir):
    os.makedirs(params_dir)

view = np.load(args.outdir + "view.npy")
opacity = np.load(args.outdir + "opacity.npy")
color = np.load(args.outdir + "color.npy")
#up_vector = args.up

vp_id = []
elevation = []
azimuth = []
file = open('angle_class.txt','r')
for line in file.readlines():
    idx, Televation, Tazimuth = line.split()
    vp_id.append(int(idx))
    elevation.append(float(Televation))
    azimuth.append(float(Tazimuth))

view_path = params_dir + "view"
opacity_path = params_dir + "opacity"
color_path = params_dir + "color"
label_path = args.outdir + "label.txt"

if not os.path.isfile(view_path):
    vof = open(view_path, "w")
    oof = open(opacity_path, "w")
    cof = open(color_path, "w")
    lof = open(label_path, "w")
    s, e = args.start, args.end
    print("converting all npy file to c++ input")
    idx = 0
    #oof.write("%d\n" %e)
    for (vs, op, cs) in tqdm(list(zip(view[:e], opacity[:e], color[:e]))):
        vof.write("%f %f %f %f\n" % (vs[0], vs[1], vs[2], vs[3]))
#        dist_min = 10
#        label = 0
#        for i in vp_id:
#            #pdb.set_trace()
#            if sphere_dist(elevation[i], azimuth[i], vs[0], vs[1]) < dist_min:
#                label = i
#                dist_min = sphere_dist(elevation[i], azimuth[i], vs[0], vs[1])
#        print(str(label), elevation[label], azimuth[label], vs[0], vs[1], str(dist_min))    
#        lof.write("%s %d\n" % (args.outdir + "imgs/vimage"+str(idx)+".bmp", label))
        for (v, o) in op:
            oof.write("%f " % o)
        oof.write("\n")
        for (v, r, g, b) in cs:
            cof.write("%f %f %f " % (r, g, b))
        cof.write("\n")
        idx = idx + 1
    vof.close()
    oof.close()
    cof.close()
    print("converting finished")

#print("call the c++ program, Good Luck~")
#
#total = args.end - args.start
#count = total // args.procs  # number of
#procs = []
#for i in range(args.procs):
#    start = args.start + i * count
#    end = args.end if i == args.procs - 1 else args.start + i * count + count
#    p = Process(target=worker,
#                args=(args.ospray_renderer, args.vti_path, view_path, opacity_path, color_path,
#                      args.outdir, args.var, args.rounds, str(up_vector[0]), str(up_vector[1]), str(up_vector[2]), str(start), str(end)))
#    procs.append(p)
#    p.start()
#
#
#for p in procs:
#    p.join()
#
## all done? meow meow meow
#if args.index:
#    from indexing import indexing
#    indexing(args.outdir, view, opacity, color)
