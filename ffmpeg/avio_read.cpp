/**
 * 这个是ffmpeg的样例代码
 * http://www.ffmpeg.org/doxygen/trunk/doc_2examples_2avio_reading_8c-example.html
 *
 * 原是c代码，这里对头文件进行了C++的特殊处理 “extern "C"{}”
 *
 */
// 编译
// g++ -std=c++11 -I /usr/local/include avio_read.cpp  -lavdevice -lavfilter \
  -lavformat -lavcodec -lswresample -lswscale -lavutil -o avio_read

// 运行
//   ./avio_read 1.mp3
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/file.h>
}

struct buffer_data {
  uint8_t *ptr;
  size_t size;  ///< size left in the buffer
};
static int read_packet(void *opaque, uint8_t *buf, int buf_size) {
  struct buffer_data *bd = (struct buffer_data *)opaque;
  buf_size = FFMIN(buf_size, bd->size);
  printf("ptr:%p size:%zu\n", bd->ptr, bd->size);
  /* copy internal buffer data to buf */
  memcpy(buf, bd->ptr, buf_size);
  bd->ptr += buf_size;
  bd->size -= buf_size;
  return buf_size;
}

int main(int argc, char *argv[]) {
  AVFormatContext *fmt_ctx = NULL;
  AVIOContext *avio_ctx = NULL;
  uint8_t *buffer = NULL, *avio_ctx_buffer = NULL;
  size_t buffer_size, avio_ctx_buffer_size = 4096;
  char *input_filename = NULL;
  int ret = 0;
  struct buffer_data bd = {0};
  if (argc != 2) {
    fprintf(stderr,
            "usage: %s input_file\n"
            "API example program to show how to read from a custom buffer "
            "accessed through AVIOContext.\n",
            argv[0]);
    return 1;
  }
  input_filename = argv[1];
  /* register codecs and formats and other lavf/lavc components*/
  /* 激活解码器、编码格式还有其他avf、avc组建*/
  av_register_all();
  /* slurp file content into buffer */
  //   av_file_map()：读取文件，并将其内容放入一个新分配的buffer中。使用av_file_unmap()来释放内存。
  ret = av_file_map(input_filename, &buffer, &buffer_size, 0, NULL);
  if (ret < 0) goto end;
  /* fill opaque structure used by the AVIOContext read callback */
  /* Opaque data type 不透明数据类型，感觉就是范型的意思 */
  bd.ptr = buffer;
  bd.size = buffer_size;
  if (!(fmt_ctx = avformat_alloc_context())) {
    ret = AVERROR(ENOMEM);
    goto end;
  }
  //指针类型的转换，C++貌似比C也要严格，强转了一下
  avio_ctx_buffer = (uint8_t *)av_malloc(avio_ctx_buffer_size);
  if (!avio_ctx_buffer) {
    ret = AVERROR(ENOMEM);
    goto end;
  }
  /**
   * AVIOContext中有以下几个变量比较重要：
   *
   * unsigned char *buffer：缓存开始位置
   * int buffer_size：缓存大小（默认32768）
   * unsigned char *buf_ptr：当前指针读取到的位置
   * unsigned char *buf_end：缓存结束的位置
   * void *opaque：URLContext结构体
   */
  avio_ctx = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size, 0, &bd,
                                &read_packet, NULL, NULL);
  if (!avio_ctx) {
    ret = AVERROR(ENOMEM);
    goto end;
  }
  fmt_ctx->pb = avio_ctx;
  ret = avformat_open_input(&fmt_ctx, NULL, NULL, NULL);
  if (ret < 0) {
    fprintf(stderr, "Could not open input\n");
    goto end;
  }
  ret = avformat_find_stream_info(fmt_ctx, NULL);
  if (ret < 0) {
    fprintf(stderr, "Could not find stream information\n");
    goto end;
  }
  av_dump_format(fmt_ctx, 0, input_filename, 0);
end:
  avformat_close_input(&fmt_ctx);
  /* note: the internal buffer could have changed, and be != avio_ctx_buffer */
  av_freep(&avio_ctx->buffer);
  av_freep(&avio_ctx);
  av_file_unmap(buffer, buffer_size);
  if (ret < 0) {
    fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
    return 1;
  }
  return 0;
}