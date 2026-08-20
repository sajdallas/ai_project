uint32_t G11_BoBaoWanBi_U32;//播报完毕标记
void F11_PlayEndCallBack();//播报完毕回调函数
void F11_BoBaoIDJiuDi0(uint32_t mID, bool mDengDai, bool mDaDuan);//语音播报ID就地播报

void F11_PlayEndCallBack(){
  //播报完毕回调函数
  G11_BoBaoWanBi_U32 = 1;
}

  //播报ID,功能号11，作者LiBai
  //参数0类型:U32，播报ID。
  //参数1类型:布尔，=True播报完再继续执行
  //参数2类型:布尔，=True打断当前播报，立即播报新ID
  //播报之前必须唤醒
void F11_BoBaoIDJiuDi0(uint32_t mID, bool mDengDai, bool mDaDuan){
  delay(200);
  enter_wakeup(20000);
  delay(200);
  G11_BoBaoWanBi_U32 = 1;//播报完毕标记置位
  //请求播报ID
  for (int i = 0; i < 20; i = i + 1) {
    if(prompt_play_by_cmd_id(mID, -1, F11_PlayEndCallBack, mDaDuan)){
      delay(200);
    }
    else{
      G11_BoBaoWanBi_U32 = 0;//如果在规定次数内请求成功，播报完毕标记复位
      break;
    }
  }
  //循环等待，重复直到有消息传来，无论消息pX是什么内容，都结束循环等待。
  //消息由播报完毕回调功能发过来，从而实现播报完毕才结束的功能。
  if(mDengDai){
    while (!G11_BoBaoWanBi_U32) {
      //每次循环制造200mS空闲，让出控制权并等待
      delay(200);
    }
  }
}