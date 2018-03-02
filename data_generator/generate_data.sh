file_path="/media/cad/Elements/tfgan/foot"
count=50000
color=("b" "w")
for ((i=1; i<=3; i++)); do	
	for ((j=0;j<=1;j++)) do				
		python render_random_meta.py foot.vti $file_path/$i-${color[j]} $count foot-$i.TF1D
		python render_volume_images.py foot.vti $file_path/$i-${color[j]}/foot_view.npy  $file_path/$i-${color[j]}/foot_opacity.npy  $file_path/$i-${color[j]}/foot_color.npy $j
	done
done
