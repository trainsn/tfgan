import sys
import os
import numpy as np
import tf_generator
import vtk
import argparse
from tqdm import tqdm


# spath fossil data
# opacity_gmm,color_gmm = tf_generator.generate_opacity_color_gmm(min_scalar_value,max_scalar_value,num_modes,begin_alpha=0.125,end_alpha=0.825)

# jet data
# opacity_gmm,color_gmm = tf_generator.generate_opacity_color_gmm(min_scalar_value,max_scalar_value,num_modes,begin_alpha=0.15,end_alpha=0.85)

# cucumber data
# opacity_gmm,color_gmm = tf_generator.generate_opacity_color_gmm(min_scalar_value,max_scalar_value,num_modes,begin_alpha=0.25,end_alpha=0.95)

# visiblemale data
# opacity_gmm, color_gmm = tf_generator.generate_opacity_color_gmm(min_scalar_value, max_scalar_value, num_modes, begin_alpha=0.15, end_alpha=0.9)


class MetaGenerator(object):
    def __init__(self, data_file_name, tf1d_filename, bg_color, scalar_field_name='Scalars_', max_zoom=2.5, begin_alpha=0.1, end_alpha=0.9):
#        volume_reader = vtk.vtkXMLImageDataReader()
#        volume_reader.SetFileName(data_file_name)
#        volume_reader.Update()
#
#        volume_data = volume_reader.GetOutput()
#        volume_data.GetPointData().SetActiveAttribute(scalar_field_name, 0)
#        self.data_range = volume_data.GetPointData().GetScalars().GetRange()

        # default options
        self.name = data_file_name + '_'
        self.min_scalar_value = 0.0
        self.max_scalar_value = 255.0 
#        self.num_cps = 5
#        self.num_colors = 5
#        self.scalar_step = (self.max_scalar_value - self.min_scalar_value) / (self.num_cps - 1)
        self.min_elevation = 5
        self.max_elevation = 165
        self.max_modes = 3
        self.max_zoom = max_zoom
        self.tf_res = 256
        self.tf1d_filename = tf1d_filename
        self.bg_color = bg_color

        self.begin_alpha = begin_alpha
        self.end_alpha = end_alpha

    def gen_view(self):
        elevation = np.random.uniform(self.min_elevation, self.max_elevation)
        azimuth = np.random.uniform(0, 360)
        roll = np.random.uniform(-10, 10)
        zoom = np.random.uniform(1, self.max_zoom)
        if self.name == 'foot_':
            roll = np.random.uniform(0, 360)
            zoom = np.random.uniform(1, self.max_zoom)
        return np.array([elevation, azimuth, roll, zoom])
    
    #add by trainsn
    #get viewpoints by sphere idx 
    def gen_view_sphere(self, n):
        vws = np.zeros((n, 4))
        ele_mean = []
        ele_count = []
        vp_sum = 0
        file = open('angle_class.txt','r')
        for line in file.readlines():
            idx, Televation, Tazimuth = line.split()
            vp_sum = vp_sum+1
            if len(ele_mean) == 0 or ele_mean[-1] != float(Televation):
                ele_mean.append(float(Televation))
                ele_count.append(1)
            else:
                ele_count[-1] = ele_count[-1]+1  
        for i in range(len(ele_count)):
            ele_count[i] = ele_count[i]/vp_sum
            
        pre = ele_count[0] * n
        idx = 0
        for i in range(n):            
            if idx == 0:
                elevation = np.random.uniform(0,(ele_mean[idx+1]+ele_mean[idx])/2)
            elif idx == len(ele_mean)-1:
                elevation = np.random.uniform((ele_mean[idx-1]+ele_mean[idx])/2,180)
            else:
                elevation = np.random.uniform((ele_mean[idx-1]+ele_mean[idx])/2,(ele_mean[idx+1]+ele_mean[idx])/2)
            azimuth = np.random.uniform(0, 360)
            roll = np.random.uniform(-10, 10)
            zoom = np.random.uniform(1, self.max_zoom)
            if self.name == 'foot_':
                roll = np.random.uniform(0, 360)
                zoom = np.random.uniform(1, self.max_zoom)  
            vws[i,:] = np.array([elevation, azimuth, roll, zoom])
            if i+1 >= pre:
               idx = idx+1
               pre = pre+ele_count[idx]*n  
        return vws
    
    def gen_op_tf(self):
        num_modes = np.random.random_integers(1, self.max_modes + 1)
        opacity_gmm = tf_generator.generate_opacity_gmm(self.min_scalar_value, self.max_scalar_value, num_modes, begin_alpha=self.begin_alpha, end_alpha=self.end_alpha)
        return tf_generator.generate_op_tf_from_op_gmm(opacity_gmm, self.min_scalar_value, self.max_scalar_value, self.tf_res, True)

    def gen_meta(self):
        num_modes = np.random.random_integers(1, self.max_modes + 1)
        #opacity_gmm, color_gmm = tf_generator.generate_opacity_color_gmm(self.min_scalar_value, self.max_scalar_value, num_modes, begin_alpha=self.begin_alpha, end_alpha=self.end_alpha)
        op, cm = tf_generator.generat_tf_from_tf1d(self.tf1d_filename, num_modes, self.bg_color, self.min_scalar_value, self.max_scalar_value, self.tf_res, True)
        return op, cm

    def gen_metas(self, n, save=False, outdir="./"):
        vws = np.zeros((n, 4))
        ops = np.zeros((n, self.tf_res, 2))
        cms = np.zeros((n, self.tf_res, 4))
        for i in tqdm(list(range(n))):
            ops[i, :, :], cms[i, :, :] = self.gen_meta()
        
        vws = self.gen_view_sphere(n)

#        for i in range(12):
#            for j in range(24):
#                elevation = i*15
#                azimuth = j*15
#                roll = 0
#                zoom = 2
#                vws[i*24+j, :] =  np.array([elevation, azimuth, roll, zoom])
#        for i in range(n):
#            ops[i, :, :]  = ops[0, :, :]
#            cms[i, :, :]  = cms[0, :, :]
        if save:
            np.save(outdir + "view", vws)
            np.save(outdir + "opacity", ops)
            np.save(outdir + "color", cms)

    def get_stg1_metas(self, n):
        vws = np.zeros((n, 4))
        ops = np.zeros((n, 2, self.tf_res))
        for i in range(n):
            vws[i, :] = self.gen_view()
            ops[i, :, :] = self.gen_op_tf().T
        return vws, ops


# TODO find opt for visiblemale dataset
class VisiblemaleMetaGenerator(MetaGenerator):
    def __init__(self, data_file_name):
        super().__init__(data_file_name)
        self.name = "visiblemale."


# TODO find opt for combustion dataset
class CombustionMetaGenerator(MetaGenerator):
    def __init__(self, data_file_name):
        super().__init__(data_file_name)
        self.name = "combustion."


def main():
    parser = argparse.ArgumentParser("python render_random_meta.py")
    parser.add_argument("dataset", help="path to vti dataset")
    parser.add_argument("outdir", default='./', help="output directory")
    parser.add_argument("n_samples", type=int, help="number of samples")
    #add by trainsn
    parser.add_argument("tf1d_filename", help="tf1d file name")
    parser.add_argument("bg_color", help="background color")
    parser.add_argument("-n", "--name", default="Scalars_", help="Scalar field name")
    parser.add_argument("-z", "--max_zoom", default=1.5, type=float, help="max zoom")
    parser.add_argument("-begin_alpha", "--begin_alpha", default=0.1, type=float, help="opacity TF domain starting point (percentage of full domain)")
    parser.add_argument("-end_alpha", "--end_alpha", default=0.9, type=float, help="opacity TF domain ending point (percentage of full domain)")

    args = parser.parse_args()
    if args.outdir[-1] != '/':
        args.outdir += '/'
    if not os.path.exists(args.outdir):
        os.mkdir(args.outdir)

    gen = MetaGenerator(args.dataset, args.tf1d_filename, scalar_field_name=args.name, bg_color=args.bg_color, max_zoom=args.max_zoom, begin_alpha=args.begin_alpha, end_alpha=args.end_alpha)
    gen.gen_metas(args.n_samples, save=True, outdir=args.outdir)


if __name__ == '__main__':
    main()
