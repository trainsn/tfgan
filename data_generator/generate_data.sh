volume=("engine" "foot" "head" "fish" "vismale" "tree")
tf1d_nums=(9 3 7 6 4 9)
count=25000
color=("b" "w")
debug_mode=$1
for ((v=5; v<6; v++)); do
	file_path=../datasets/${volume[v]}
	#file_path=/media/cad/0C66F84266F82DD8/tfgan/${volume[v]}
	if [ $debug_mode == 1 ]
	then 
		rm -r $file_path
	fi
	mkdir -p $file_path
	tf1d_num=${tf1d_nums[v]}
	#tf1d_num=1	
	i=1
	#while (( $i<=$tf1d_num ))
	for (( i=$tf1d_num;i<=$tf1d_num;i++ ))
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
	done

	all_opacity_b=$file_path/opacity_b.txt
	all_color_b=$file_path/color_b.txt
	all_opacity_w=$file_path/opacity_w.txt
	all_color_w=$file_path/color_w.txt 
	touch $all_opacity_b
	echo `expr $count \* $tf1d_num` > $all_opacity_b
	touch $all_opacity_w
	echo `expr $count \* $tf1d_num` > $all_opacity_w
	touch $all_color_b
	touch $all_color_w
	
	for (( i=$tf1d_num;i<=$tf1d_num;i++ ))
	do					
		cat $file_path/$i-b/params/opacity.txt >> $all_opacity_b
		cat $file_path/$i-b/params/color.txt >> $all_color_b
		cat $file_path/$i-w/params/opacity.txt >> $all_opacity_w
		cat $file_path/$i-w/params/color.txt >> $all_color_w
	done
done

