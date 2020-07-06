# AvIOContext
```c
avioCtx->buffer // 即io上下文中那个小buffer,通过avio_alloc_context()来分配io上下文时作为参数传递

avioCtx->buffer_size // 小buffer的大小
avioCtx->buf_ptr // io上下文的小buffer中的数据当前被消耗的位置
avioCtx->buf_end // io上下文的小buffer数据的结束位置
avioCtx->opaque  // ** 是一个自定义的结构体,存储视频大buffer的信息,如buffer开始位置,视频buffer长度,这个结构会回传给 read_packet、write_packet、seek回调函数！！！ **,
avioCtx->read_packet  // 需要自己实现的一个 `用视频大buffer数据` 填充 `io上下文小buffer`的回调函数
avioCtx->write_packet //自己实现 把io上下文中的小buffer数据写到某处的回调函数
avioCtx->seek // 也是自己实现的 用来在视频大buffer中seek的函数
```
## seek
ffmpeg 中 seek 功能通过 av_seek_frame()方法来进行
* 回调函数结构，callback func
    * `int64_t(* seek )(void *opaque, int64_t offset, int whence)`

* opaque: 同 read_packet 回调,原始视频 buffer 信息的结构体
    * 就是初始化中的文件stream，或者其他源的指针，用指针就能强转成需要的对象、数据

* offset: 要 seek 到的位置,可以是相对原始视频的起始位置,可以是相对 io 上下文小 buffer 的起始位置,取决于 whence
* whence: seek 的类型,取值为 AVSEEK_SIZE 、SEEK_CUR 、SEEK_SET、SEEK_END,
    * AVSEEK_SIZE: 不进行 seek 操作,而是要求返回 视频 buffer 的长度大小
    * SEEK_CUR: 表示 offset 是相对 io 上下文小 buffer 开始位置的
    * SEEK_SET: 表示 offset 是相对 原始 buffer 开始位置的
    * SEEK_END: 表示 offset 是相对 原始 buffer 结束位置的

通过 debug av_seek_frame --> seek_frame_internal ---> seek_frame_byte --->avio_seek() 发现对 iocontext 自定义的 seek 方法是在 avio_seek() 中使用的。
发现 avio_seek 中调用 ioContext->seek()时 whence 只会传递 AVSEEK_SIZE 或 SEEK_SET

所以自定义 io seek 函数实现如下即可:
```c
static int64_t seek_in_buffer(void *opaque, int64_t offset, int whence){
  BufferData *bd = (BufferData *)opaque;
  int64_t ret = -1;

  // printf("whence=%d , offset=%lld , file_size=%ld\n", whence, offset, bd->file_size);
  switch (whence)
  {
  case AVSEEK_SIZE:
    ret = bd->file_size;
    break;
  case SEEK_SET:
    bd->ptr = bd->ori_ptr + offset;
    bd->size = bd->file_size - offset;
    ret = bd->ptr;
    break;
  }
  return ret;
}
```