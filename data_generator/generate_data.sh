volume=("engine" "foot")
tf1d_nums=(9 3)
count=50
color=("b" "w")
#debug_mode=$1
for ((v=0; v<1; v++)); do
	file_path=/media/cad/Elements/tfgan/${volume[v]}
	tf1d_num=${tf1d_nums[v]}
	#tf1d_num=1	
	i=1
	while (( $i<=$tf1d_num ))
	do	
		for ((j=0;j<=1;j++)) do	
			#if [ $debug_mode == 1 ]
			#then 
			#	rm -r $file_path/$i-${color[j]}/imgs
			#fi			
			python render_random_meta.py ${volume[v]} $file_path/$i-${color[j]} $count TF1D/${volume[v]}-$i.TF1D $j
			python ospray_launcher.py $file_path/$i-${color[j]} -e $count		
			#rawRender/build/rawRender $file_path/$i-${color[j]} volume_file ${volume[v]} $count $j
			#python render_volume_images.py foot.vti $file_path/$i-${color[j]}/view.npy  $file_path/$i-${color[j]}/opacity.npy  $file_path/$i-${color[j]}/color.npy $j		
		done
		let "i++"
	done
done

