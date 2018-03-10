import sys
import os

volume = ['engine','foot', 'head']
tf1d_nums = [9, 3, 7]
count = 25000
color = ['b', 'w']
debug_mode = 1
for v in range(2,3):
	file_path='C:\\tfgan\\' + volume[v]
	if debug_mode == 1:
		os.system('rd /s ' + file_path)
	os.system("mkdir " + file_path)
	
	tf1d_num = tf1d_nums[v]
	#tf1d_num = 1
	for i in range(tf1d_num):
		for j in range(2):
			meta_cmd = 'python render_random_meta.py ' + volume[v] + ' ' + file_path+'\\'+str(i+1)+'-'+color[j] + ' ' + str(count) + ' TF1D\\' + volume[v]+'-'+str(i+1)+ '.TF1D ' + str(j)
			print(meta_cmd)
			os.system(meta_cmd) 
			convert_cmd = 'python ospray_launcher.py ' + file_path + '\\' + str(i+1) + '-' + color[j] + ' -e ' + str(count)
			print(convert_cmd)
			os.system(convert_cmd)
	for i in range(2):		
		all_opacity_file = file_path + '\\opacity_' + color[i] + '.txt'
		all_color_file = file_path + '\\color_' + color[i] + '.txt'
		os.system('echo ' + str(count*tf1d_num) + '>' + all_opacity_file)
		os.system('cd.>' + all_color_file)
		for j in range(tf1d_num):
			opacity_file = file_path + '\\' + str(j+1) + '-' + color[i] + '\\params\\opacity.txt'
			color_file = file_path + '\\' + str(j+1) + '-' + color[i] + '\\params\\color.txt'
			copy_cmd = 'type ' + opacity_file + '>>' + all_opacity_file
			#print(copy_cmd)
			os.system(copy_cmd)
			copy_cmd = 'type ' + color_file + '>>' + all_color_file
			os.system(copy_cmd)
	