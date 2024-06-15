==========================================================================
【  ソフト名  】MozJpegGUI
【 バージョン 】1.8.0
【 ファイル名 】MozJpegGUI1_8_0_x64.zip
【   作者名   】どな
【ホームページ】http://donadona.hatenablog.jp/
【動作確認機種】Windows11 Professional
【  必要環境  】Windows10以降のWindowsシステム
【  解凍方法  】ZIPにて解凍
【 ｿﾌﾄｳｪｱ種別 】フリーウェア
【    転載    】禁止
【　 連絡先 　】nibasha@hotmail.com
==========================================================================
Copyright(c) 2024 どな All rights reserved. 

【このソフトでできること】
大量の画像ファイルを高速かつ高圧縮率でjpeg形式に変換する

【このソフトの説明】
・このソフトは高画質と高圧縮率を高いレベルで両立可能なmozjpegライブラリを
　用いて、様々な画像ファイルをjpeg形式に変換します。
・近年普及してきた多数のコアを持つCPUの性能を活かし、複数のファイルを同時
　に変換することで高速な変換を実現します。この特徴は変換するファイルの数が
　多いほど顕著になります。
・複数の変換設定を保存できます。そのため、用途に応じた変換設定の変更が手軽
　にできます。
・変換元ファイルとして、.bmp, .gif, .jpg, .jpeg, .png, .ppm, .tgaに対応し
　ています。
・変換元ファイルはドラッグ＆ドロップにより指定可能。また、特定のフォルダに
　ある画像ファイルを全て取り込むこともできます。
・元ファイルによっては、変換後の方がファイルサイズが大きくなることがありま
　す。「大きさ優先」にチェックを入れることで、このような場合には元ファイル
　をそのまま出力先フォルダにコピーするようにできます。
　なお、この現象が起きるのは元ファイルがモノクロ画像の場合などです。jpeg
　形式はモノクロ画像に非対応なため、モノクロ画像はグレースケールに変換され
　てしまい必然的にファイルサイズが大きくなります。
・超々高性能CPUをお使いの方にはすみませんが、使用する論理プロセッサ数の上
　限は64と思われます（CPUだけで数十万円するため未検証です@2020年現在）。
・32bit版と64bit版があります。32bit版はどのWindows10でも動きます。
　一方、64bit版は64bitのWindows10でしか動きませんが、およそ8%ほど高速なよ
　うです。
・本圧縮ファイルは64bit版です。

【インストール手順】
適当なフォルダに、本圧縮ファイルの中身を全て解凍してください。

【アンインストール方法】
「インストール手順」にて解凍したファイルおよびフォルダを削除してください。

【使用方法】
1. 解凍した先にあるMozJpegGUI.exeを実行してください。
2. ソフトウェアが起動したら、変換したいファイルをソフトウェア左側にある白
　 い箱にドラッグアンドドロップしてください。
　 または、「フォルダから追加」ボタンを押して、変換したいファイルが入って
　 いるフォルダを選んでください。なお、フォルダを選ぶダイアログではファイ
　 ルが一つも表示されませんが、これはWindowsの仕様です。
3. 「保存先」の右にある「参照...(O)」ボタンを押して、保存先フォルダを選択
　 してください。
4. 必要に応じ、「大きさ優先」や「上書き保存」、「圧縮品質」などを設定して
　 ください。これらの設定はソフトウェア終了時や「設定管理」から別の設定を
　 選んだときに保存されます。
5. 画像変換の準備が整ったら、「変換(C)」ボタンを押して変換を開始してくだ
　 さい。
6. 進捗状況ダイアログが表示されますので、進捗状況が100%相当まで進むまでお
　 待ちください。
　 なお、「読み込みバッファ」は処理高速化のため先読み済みのファイル数を、
　 「CPU」は現在同時に計算している圧縮処理数を示します。
　 「読み込みバッファ」「CPU」が共に0%に近いときはファイルの読み込みがボ
　 トルネックになっています。一方、CPUが100%の場合はCPUがボトルネックにな
　 っていることを示します。
7. 変換が完了すると、変換完了を示すメッセージが表示されます。
8. 別のファイルに対し続けて変換をしたい場合は、「クリア」ボタンを押してフ
　 ァイルの一覧をクリアしてから2.に戻ってください。

【改訂履歴】
Ver. 1.8.0:
  1. バグ修正: メタデータが無い場合(例: ビットマップ)、エラーメッセージを表示し変換を停止してしまっていた。
  2. 機能追加：使用するCPU数を制限可能になった。メモリ使用量を減らすのに有用。
  3. バージョン番号に3桁目を追加。非常に小さなバグ修正のときに値が1増える。
Ver. 1.7:
  1. バグ修正: AVX512非対応CPUで実行するとMozJpegGUIが突然落ちる。
Ver. 1.6:
  バグにより欠番。
Ver. 1.5:
  1. MozJpegGUI.iniの[Common]の下に下記を追記すると、バグ報告に有用なログを保存する。
     Output debug log=1
Ver.1.4:
　・内蔵ライブラリを更新。
     zlib 1.2.11 -> 1.3
     libpng 1637 -> 1640
     mozjpeg 3.3.1 -> 4.1.1
　・jpeg以外の形式から一部のメタデータをコピーするように変更。
　　jpegからはすべてのメタデータをコピーする。
　・.gif形式読み込み時にクラッシュする確率を低減。
Ver.1.3:
　・元ファイルと同じ場所にファイルを保存する機能を追加。
　・英語版においてGUIの文字が一部隠れている問題を修正。
Ver.1.2:
　・バグ修正：ファイル名に空白文字が含まれる場合、正しくないファイル名で保存してしまう
Ver.1.1:
　・mozjpeg内でエラーが起きた場合、プログラムを自動終了せずにエラーメッセージを表示するように変更。
　・まれに存在するトップダウン形式のBMPファイルに対応。
Ver.1.0:
　・公開。


【著作権・免責について】
    本ソフトウェアの各ファイルの著作権は、作者に属します。
    
    本ソフトウェアは、著作権者およびコントリビューターによって「現状のまま」
　　提供されており、明示黙示を問わず、商業的な使用可能性、および特定の目的に
　　対する適合性に関する暗黙の保証も含め、またそれに限定されない、いかなる
　　保証もありません。著作権者もコントリビューターも、事由のいかんを問わず、
　　損害発生の原因いかんを問わず、かつ責任の根拠が契約であるか厳格責任であるか
　　（過失その他の）不法行為であるかを問わず、仮にそのような損害が発生する
　　可能性を知らされていたとしても、本ソフトウェアの使用によって発生した（代替
　　品または代用サービスの調達、使用の喪失、データの喪失、利益の喪失、業務の
　　中断も含め、またそれに限定されない）直接損害、間接損害、偶発的な損害、特別
　　損害、懲罰的損害、または結果損害について、一切責任を負わないものとします。 
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
    PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

　　本ソフトウェアはIndependent JPEG Groupの成果の一部を使用しております。

    This software is based in part on the work of the Independent JPEG Group.

　　本ソフトウェアはmozjpeg、libpng、zlibの各ライブラリを使用しております。
　　そのため、license.mdに記載の条項を各ライブラリ部分について継承します。

    As this software uses mozjpeg, libpng and zlib libraries, sub-module of
    the software inherites their licenses as shown in license.md.
　