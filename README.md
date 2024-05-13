# H3FONTX2: FONTX2 Library
## 出来る事
* [FatFs](http://elm-chan.org/fsw/ff/)かバッファからFONTX2形式のフォントを読み込む
* ANK/Shift JISの文字コードから画像を得る
* バッファに文字列の描画を行う
    * 全角・半角フォントを組み合わせて描画する
    * 可能なら改行を行う

## 使用例
```c
//FatFsでフォントファイルの読み込み
FIL hanFile;
FIL zenFile;
f_open(&hanFile, "msg_han.fnt", FA_OPEN_EXISTING | FA_READ);
f_open(&zenFile, "msg_zen.fnt", FA_OPEN_EXISTING | FA_READ);
//初期化
FONTX2_handler_t hHanF;
FONTX2_handler_t hZenF;
FONTX2_init(&hHanF, &hanFile, 1);
FONTX2_init(&hZenF, &zenFile, 1);

//描画バッファを用意・初期化
uint8_t canvas[800*32] = {0};
FONTX2_str_handler_t strHandler = {0};
FONTX2_str_init(&strHandler, 1, &hHanF, &hZenF, 800, 32, canvas);
//描画
FONTX2_str_puts(&strHandler, "赤色:0x01", 0x01);
FONTX2_str_puts(&strHandler, "緑色:0x02", 0x02);
FONTX2_str_puts(&strHandler, "青色:0x03", 0x03);

//フレームバッファにコピー(ライブラリ外の関数)
DMA2D_DrawL8(canvas, (uint8_t*)FBStart, 0, 0, 800, 32);
```
![20240513_115413](https://github.com/haru3me/H3FONTX2/assets/121174745/81d53a73-4d50-4903-8405-c8dbe2310263)

## おまけ
* ImHex用ヘッダ定義
```c
struct fontx2_codeblock{
    le u16 cbstart;
    le u16 cbend;
};

struct fontx2_header{
    char magick[6];
    char font_name[8];
    u8 font_width;
    u8 font_height;
    u8 char_code;
    u8 num_codeblock;
    fontx2_codeblock codeblocks[num_codeblock];
};

fontx2_header hdr @ 0x00;
```
