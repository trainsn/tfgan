file_path=/media/cad/Elements/tfgan/foot
count=50
color=("b" "w")
debug_mode=$1
for ((i=1; i<=1; i++)); do	
	for ((j=0;j<=1;j++)) do	
		if [ $debug_mode == 1 ]
		then 
			rm -r $file_path/$i-${color[j]}/imgs
		fi			
		python render_random_meta.py foot $file_path/$i-${color[j]} $count TF1D/foot-$i.TF1D $j
		python ospray_renderer/ospray_launcher.py $file_path/$i-${color[j]} -e $count		
		rawRender/build/rawRender $file_path/$i-${color[j]} volume_file foot $count $j
		#python render_volume_images.py foot.vti $file_path/$i-${color[j]}/view.npy  $file_path/$i-${color[j]}/opacity.npy  $file_path/$i-${color[j]}/color.npy $j
		
	done
done

