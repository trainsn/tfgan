#include <vtkDICOMImageReader.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolume.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageCast.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkBMPReader.h>
#include <vtkVolume16Reader.h>
#include <vtkRayCastImageDisplayHelper.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkWindowToImageFilter.h>
#include <vtkBMPWriter.h>

#include <iostream>
#include <sstream>

using namespace std;

bool fexists(const string& name){
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        cout << name << " already exists, skip." << endl;
        return true;
    } else {
        return false;
    }
}

int main(int argc, char **argv)
{
    cout << "start programming" << endl;
    string base_dir = argv[1];
    string volume_type = argv[2];
    if (base_dir[base_dir.size() - 1] != '/')
        base_dir += "/";

    string filename = (base_dir + volume_type + ".vifo");
    FILE *fp = fopen(filename.c_str(), "r");
    //gdbcout << filename << endl;

    char dataFile[1024];
    int xiSize, yiSize, ziSize;			// size of the original volume
    double xSpace, ySpace, zSpace;		// spacing size of the original volume
    fscanf(fp, "%d %d %d\n", &xiSize, &yiSize, &ziSize);
    //cout << xiSize << yiSize << ziSize << endl;
    fscanf(fp, "%lf %lf %lf\n", &xSpace, &ySpace, &zSpace);
    fscanf(fp, "%s", dataFile);
    //cout << dataFile << endl;
    char raw_path[1024];
    strcpy(raw_path, base_dir.c_str());
    strcat(raw_path, dataFile);

    vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();//设置绘制者(绘制对象指针)
	vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();//设置绘制窗口
	renWin->AddRenderer(ren);//将绘制者加入绘制窗口
	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();//设置绘制交互操作窗口的
	iren->SetRenderWindow(renWin);//将绘制窗口添加到交互窗口

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();//交互摄像机
	iren->SetInteractorStyle(style);//style为交互模式
	vtkSmartPointer<vtkImageReader> reader = vtkSmartPointer<vtkImageReader>::New();

    reader->SetFileName((const char*)raw_path);
	reader->SetFileDimensionality(3);//设置显示图像的维数
	reader->SetDataScalarType(VTK_UNSIGNED_CHAR);//VTK_UNSIGNED_short将数据转换为unsigned char型

    reader->SetDataExtent(0, xiSize-1, 0, yiSize-1, 0, ziSize-1);
    reader->SetDataSpacing(xSpace, ySpace, zSpace); //设置像素间间距
    //reader->SetDataExtent(0, 255, 0, 255, 0, 255);
    //reader->SetDataSpacing(1, 1, 1); //设置像素间间距
	reader->SetDataOrigin(0.0, 0.0, 0.0);//设置基准点，（一般没有用）做虚拟切片时可能会用的上
	reader->Update();

	vtkSmartPointer<vtkImageCast> readerImageCast = vtkSmartPointer<vtkImageCast>::New();//数据类型转换
	readerImageCast->SetInputConnection(reader->GetOutputPort());
	readerImageCast->SetOutputScalarTypeToUnsignedChar();
	readerImageCast->ClampOverflowOn();//阀值
	
	//设置不透明度传递函数//该函数确定各体绘像素或单位长度值的不透明度
	vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();//一维分段函数变换
	opacityTransferFunction->AddPoint(20, 0.0);
	opacityTransferFunction->AddPoint(255, 0.2);

	//设置颜色传递函数//该函数确定体绘像素的颜色值或者灰度值
	vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
	colorTransferFunction->AddRGBPoint(0.0, 0.0, 0.5, 0.0);//添加色彩点（第一个参数索引）
	colorTransferFunction->AddRGBPoint(60.0, 1.0, 0.0, 0.0);
	colorTransferFunction->AddRGBPoint(128.0, 0.2, 0.1, 0.9);
	colorTransferFunction->AddRGBPoint(196.0, 0.27, 0.21, 0.1);
	colorTransferFunction->AddRGBPoint(255.0, 0.8, 0.8, 0.8);

	vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	//设定一个体绘容器的属性
	volumeProperty->SetColor(colorTransferFunction);//设置颜色
	volumeProperty->SetScalarOpacity(opacityTransferFunction);//不透明度
        volumeProperty->ShadeOff();//影阴
	volumeProperty->SetInterpolationTypeToLinear();//直线与样条插值之间逐发函数
        volumeProperty->SetAmbient(1.0);//环境光系数
        volumeProperty->SetDiffuse(0.5);//漫反射
        volumeProperty->SetSpecular(0.75);//高光系数
        volumeProperty->SetSpecularPower(40); //高光强度

        vtkSmartPointer<vtkVolumeRayCastCompositeFunction> compositeFunction = vtkSmartPointer<vtkVolumeRayCastCompositeFunction>::New();
	//运行沿着光线合成
	//定义绘制者
        /*vtkVolumeRayCastMapper *volumeMapper = vtkVolumeRayCastMapper::New(); //体绘制器
        volumeMapper->SetVolumeRayCastFunction(compositeFunction); //载入绘制方法
	volumeMapper->SetInputConnection(readerImageCast->GetOutputPort());//图像数据输入
	volumeMapper->SetNumberOfThreads(3);*/
        vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
        volumeMapper->SetBlendModeToComposite(); // composite first
        volumeMapper->SetInputConnection(readerImageCast->GetOutputPort());
        volumeMapper->SetRequestedRenderModeToGPU();
	//定义Volume
	vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();//表示透示图中的一组三维数据
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);//设置体属性
	
	ren->AddVolume(volume);//将Volume装载到绘制类中

    //设置相机
    float vol_cen[3];
    vol_cen[0] = 0.5f * xiSize;
    vol_cen[1] = 0.5f * yiSize;
    vol_cen[2] = 0.5f * ziSize;

    float vol_diag[3];
    vol_diag[0] = xiSize;
    vol_diag[1] = yiSize;
    vol_diag[2] = ziSize;

    vtkSmartPointer<vtkCamera> vtk_cam = vtkCamera::New();
    //double pos[3] = {vol_cen[0], 2 * -(vol_max[1] - vol_cen[1]), vol_cen[2]};
    double pos[3] = {vol_cen[0], vol_cen[1], vol_cen[2] +  0.6 *
        sqrt(vol_diag[0]*vol_diag[0]+vol_diag[1]*vol_diag[1]+vol_diag[2]*vol_diag[2])};
    vtk_cam->SetPosition(pos);
    double foc[3] = {vol_cen[0], vol_cen[1], vol_cen[2]};
    vtk_cam->SetFocalPoint(foc);
    double up[3] = {0.0,1.0,0.0};
    vtk_cam->SetViewUp(up);
    vtk_cam->SetViewAngle(75.);

    vtk_cam->Elevation(-85.f);

    string param_file = base_dir + "params/view";
    FILE *view_fp = fopen(param_file.c_str(), "r");
    double elevation, azimuth, roll, zoom;
    int config_id = 0;

    stringstream ss;
    string img_file_name;
    string img_dir = base_dir + "imgs/";
    if (!vtk_dir->FileIsDirectory(img_dir.c_str()))
        vtk_dir->MakeDirectory(img_dir.c_str());
    while (fscanf(view_fp, "%lf%lf%lf%lf", &elevation, &azimuth, &roll, &zoom)!=EOF){
        vtk_cam->Elevation(elevation);
        vtk_cam->Azimuth(azimuth);
        vtk_cam->Roll(roll);
        vtk_cam->Zoom(zoom);

        vtk_cam->GetPosition(pos);
        vtk_cam->GetFocalPoint(foc);
        vtk_cam->OrthogonalizeViewUp();
        vtk_cam->GetViewUp(up);

        ren->SetActiveCamera(vtk_cam);
        ren->SetBackground(1, 1, 1);
        renWin->SetSize(256, 256);//设置背景颜色和绘制窗口大小
        renWin->Render();//窗口进行绘制

        // Screenshot
        vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter =
          vtkSmartPointer<vtkWindowToImageFilter>::New();
        windowToImageFilter->SetInput(renWin);
        windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
        windowToImageFilter->ReadFrontBufferOff(); // read from the back buffer
        windowToImageFilter->Update();

        ss << img_dir << "vimage" << config_id << ".bmp";
        img_file_name = ss.str();
        ss.str("");
        ss.clear();

        vtkSmartPointer<vtkBMPWriter> writer =
          vtkSmartPointer<vtkBMPWriter>::New();
        writer->SetFileName(img_file_name.c_str());
        writer->SetInputConnection(windowToImageFilter->GetOutputPort());
        writer->Write();

        ren->ResetCameraClippingRange();

        vtk_cam->Zoom(1.0f/zoom);
        vtk_cam->Roll(-roll);
        vtk_cam->Azimuth(-azimuth);
        vtk_cam->Elevation(-elevation);
        config_id++;
    }

}
