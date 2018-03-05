#include <vtkDICOMImageReader.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
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
#include <vtkSmartVolumeMapper.h>
#include <vtkDirectory.h>
#include <vtkLightCollection.h>
#include <vtkLight.h>
#include <vtkLightActor.h>
#include <vtkSphereSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

#include <iostream>
#include <sstream>
#include <random>
#include <vector>

using namespace std;
const int res = 256;

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
    if (argc !=6){
        cout << "Usage base_dir raw_dir vifo_file render_num bg_color" << endl;
        exit(1);
    }
    //cout << "start programming" << endl;
    string base_dir = argv[1];
    string raw_dir = argv[2];
    string volume_type = argv[3];
    int render_num = stoi(argv[4]);
    float bg_color = stof(argv[5]);
    if (base_dir[base_dir.size() - 1] != '/')
        base_dir += "/";
    if (raw_dir[raw_dir.size() - 1] != '/')
        raw_dir += "/";

    string filename = (raw_dir + volume_type + ".vifo");
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
    strcpy(raw_path, raw_dir.c_str());
    strcat(raw_path, dataFile);

    vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();//设置绘制者(绘制对象指针)
    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();//设置绘制窗口
    renWin->AddRenderer(ren);//将绘制者加入绘制窗口
    /*vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();//设置绘制交互操作窗口的
    iren->SetRenderWindow(renWin);//将绘制窗口添加到交互窗口

    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();//交互摄像机
    iren->SetInteractorStyle(style);//style为交互模式*/
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

    vtkSmartPointer<vtkImageData> imageData =
       vtkSmartPointer<vtkImageData>::New();
    imageData->ShallowCopy(reader->GetOutput());

    vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();

    vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
    volumeMapper->SetBlendModeToComposite(); // composite first
    volumeMapper->SetInputData(imageData);
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
    double pos[3] = {vol_cen[0], vol_cen[1], vol_cen[2] +   0.85 *
        sqrt(vol_diag[0]*vol_diag[0]+vol_diag[1]*vol_diag[1]+vol_diag[2]*vol_diag[2])};
    vtk_cam->SetPosition(pos);
    double foc[3] = {vol_cen[0], vol_cen[1], vol_cen[2]};
    vtk_cam->SetFocalPoint(foc);
    double up[3] = {0.0,1.0,0.0};
    vtk_cam->SetViewUp(up);
    vtk_cam->SetViewAngle(75.);

    vtk_cam->Elevation(-85.f);

    // Create a sphere
    /*vtkSmartPointer<vtkSphereSource> sphereSource =
      vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetCenter(vol_cen[0], vol_cen[1], vol_cen[2]);
    sphereSource->SetRadius(10.0);

    vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkSmartPointer<vtkActor> actor =
      vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    ren->AddActor(actor);*/

    string view_param_file = base_dir + "params/view";
    FILE *view_fp = fopen(view_param_file.c_str(), "r");
    double elevation, azimuth, roll, zoom;
    int config_id = 0;

    stringstream ss;
    string img_file_name;
    string img_dir = base_dir + "imgs/";

    vtkSmartPointer<vtkDirectory> vtk_dir = vtkDirectory::New();
    if (!vtk_dir->FileIsDirectory(img_dir.c_str()))
        vtk_dir->MakeDirectory(img_dir.c_str());

    string opacity_param_file = base_dir + "params/opacity";
    FILE *opacity_fp = fopen(opacity_param_file.c_str(), "r");
    string color_param_file = base_dir + "params/color";
    FILE *color_fp = fopen(color_param_file.c_str(), "r");

    default_random_engine e;
    uniform_real_distribution<double> dis_diffuse(0,1);
    uniform_real_distribution<double> dis_specular(0,1);
    uniform_real_distribution<double> dis_specularPower(10, 128);
    vector<double> diffuse;
    vector<double> specular;
    vector<double> specularPower;
    for (int i=0; i<render_num; i++){
        diffuse.push_back(dis_diffuse(e));
        specular.push_back(dis_specular(e));
        specularPower.push_back(dis_specularPower(e));
    }

    uniform_real_distribution<double> dis_env_light(0,2);


    while (fscanf(view_fp, "%lf%lf%lf%lf", &elevation, &azimuth, &roll, &zoom)!=EOF){
        double opacity, r, g, b;
        //设置不透明度传递函数//该函数确定各体绘像素或单位长度值的不透明度
        vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();//一维分段函数变换
        for (int i=0; i< res;i++){
            fscanf(opacity_fp, "%lf", &opacity);
            opacityTransferFunction->AddPoint((double)i, opacity);
        }

        //设置颜色传递函数//该函数确定体绘像素的颜色值或者灰度值
        vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
        for (int i=0; i< res;i++){
            fscanf(color_fp, "%lf%lf%lf", &r, &g, &b);
            colorTransferFunction->AddRGBPoint((double)i, r, g, b);
        }

        volumeProperty->SetColor(colorTransferFunction);//设置颜色
        volumeProperty->SetScalarOpacity(opacityTransferFunction);//不透明度

        //set shade
        //设定一个体绘容器的属性
        volumeProperty->ShadeOn();//影阴
        volumeProperty->SetInterpolationTypeToLinear();//直线与样条插值之间逐发函数
        volumeProperty->SetAmbient(1.0);//环境光系数
        volumeProperty->SetDiffuse(diffuse[config_id]);//漫反射
        volumeProperty->SetSpecular(specular[config_id]);//高光系数
        volumeProperty->SetSpecularPower(specularPower[config_id]); //高光强度
        cout << diffuse[config_id] << " " << specular[config_id] << " " << specularPower[config_id] << endl;

        vtk_cam->Elevation(elevation);
        vtk_cam->Azimuth(azimuth);
        vtk_cam->Roll(roll);
        vtk_cam->Zoom(zoom);

        vtk_cam->GetPosition(pos);
        vtk_cam->GetFocalPoint(foc);
        vtk_cam->OrthogonalizeViewUp();
        vtk_cam->GetViewUp(up);
        if (config_id < render_num / 2){
            vtk_cam->ParallelProjectionOn();
            vtk_cam->SetParallelScale(135.24329927948372);
        }else {
            vtk_cam->ParallelProjectionOff();
        }

        ren->SetActiveCamera(vtk_cam);
        ren->SetBackground(bg_color, bg_color, bg_color);
        renWin->SetOffScreenRendering(1);
        renWin->SetSize(256, 256);//设置背景颜色和绘制窗口大小
        renWin->Render();//窗口进行绘制
        //iren->Initialize();
        //iren->Start();//初始化并进行交互绘制

        //set light
        vtkLightCollection *lights = ren->GetLights();
        int lightIndex = 0;
        vtkLight *light = nullptr;
        for (lights->InitTraversal(), light = lights->GetNextItem();
              light != nullptr; light = lights->GetNextItem(), lightIndex++)
        {
            /*cout << lightIndex << " ";
            if (light->LightTypeIsHeadlight())
                cout << "HeadLight" << " ";
            else if (light->LightTypeIsSceneLight())
                cout << "SceneLight" << " ";
            cout << light->GetIntensity() << endl;*/
            light->SetIntensity(1);
        }
        //std::cout << "Originally there are " << lights->GetNumberOfItems() << " lights." << std::endl;


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
        if (fexists(img_file_name)) {
            config_id++;
            continue;
        }

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
