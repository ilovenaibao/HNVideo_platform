����ffmpegԴ�룺

  ��c:\��Ŀ¼Ϊ��
  1. ��װmingw-msys-bundle;
  2. ����������
     1) ��ѹyasmԴ�뵽C:\yasm
        ���ñ�����·��Ϊyasm_out, ��������yasm binary�ļ�
        ./configure --prefix=/c/yasm_out; make; make install;
        ����yasm��������:
        export PATH="/c/yasm_out/bin:$PATH"
     2) ��ѹx264Դ�뵽C:\x264
        ���ñ�����·��Ϊx264_out, ��������x264 binary�ļ�
        ./configure --enable-shared --disable-asm --prefix=/c/x264_out; make; make install
        ����x264��������:
        export PATH="/c/x264_out/bin:$PATH"
        export C_INCLUDE_PATH="$C_INCLUDE_PATH:/c/x264_out/include"
        export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/c/x264_out/lib:��
   3. ����ffmpegԴ��
      1) ��ѹffmpegԴ�뵽C:\ffmpeg
         ./configure --enable-shared --disable-static --enable-memalign-hack --enable-libx264 --enable-gpl --prefix=/c/ffmpeg_out
         �������Ҫ����x264�⣬����ֱ��
         ./configure --enable-memalign-hack --enable-shared
         

Note:
    1. �ڼ���libx264��ʱ����ܻ�����Ҳ���x264.h�Ĵ���, �������ڱ�����û���ҵ�include/lib�ļ�����
       ./configure --enable-shared --disable-static --enable-memalign-hack --enable-libx264 --enable-gpl --prefix=/c/ffmpeg_out ��Ҫ���� --extra-cflags=-I/c/x264/include --extra-ldflags=-L/c/x264/lib
         

        
     