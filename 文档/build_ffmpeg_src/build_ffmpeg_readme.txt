编译ffmpeg源码：

  以c:\根目录为例
  1. 安装mingw-msys-bundle;
  2. 编译三方库
     1) 解压yasm源码到C:\yasm
        设置编译结果路径为yasm_out, 编译生成yasm binary文件
        ./configure --prefix=/c/yasm_out; make; make install;
        导入yasm环境变量:
        export PATH="/c/yasm_out/bin:$PATH"
     2) 解压x264源码到C:\x264
        设置编译结果路径为x264_out, 编译生成x264 binary文件
        ./configure --enable-shared --disable-asm --prefix=/c/x264_out; make; make install
        导入x264环境变量:
        export PATH="/c/x264_out/bin:$PATH"
        export C_INCLUDE_PATH="$C_INCLUDE_PATH:/c/x264_out/include"
        export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/c/x264_out/lib:”
   3. 编译ffmpeg源码
      1) 解压ffmpeg源码到C:\ffmpeg
         ./configure --enable-shared --disable-static --enable-memalign-hack --enable-libx264 --enable-gpl --prefix=/c/ffmpeg_out
         如果不需要最新x264库，可以直接
         ./configure --enable-memalign-hack --enable-shared
         

Note:
    1. 在加入libx264的时候可能会出现找不到x264.h的错误, 这是由于编译器没有找到include/lib文件所以
       ./configure --enable-shared --disable-static --enable-memalign-hack --enable-libx264 --enable-gpl --prefix=/c/ffmpeg_out 需要加入 --extra-cflags=-I/c/x264/include --extra-ldflags=-L/c/x264/lib
         

        
     