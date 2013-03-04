Download NPAPI Plugin for chrome, Windows
=========================================

■ソースコードのコンパイル
--------------------------

使い方は[readme.md](https://github.com/PekeTaichi/downloadNPAPI/blob/master/readme.md)を見てください。

●環境の構築
------------
・まずNPAPIはC++なのでC++コンパイラが必要です。  
 **Visual C++ 2010 Express** を以下からダウンロードします。  
[http://www.microsoft.com/visualstudio/jpn/downloads#d-2010-express](http://www.microsoft.com/visualstudio/jpn/downloads#d-2010-express)  
試用は30日間でそれ以降も利用する場合は無料登録が必要です。  

・NPAPIを作るのにnixysaを利用しているのでpythonが必要です。  
 **Python 2.6.6** を以下からダウンロードします。  
[http://www.python.org/getit/releases/2.6.6/](http://www.python.org/getit/releases/2.6.6/)  

pywin32モジュールを使うので以下から「 **pywin32-216.win32-py2.6.exe** 」をダウンロードします。  
[http://sourceforge.net/projects/pywin32/files/pywin32/Build216/](http://sourceforge.net/projects/pywin32/files/pywin32/Build216/)  

上記3つを全てインストールすればNPAPIのコンパイル環境構築は終わりです。  
もしエラーでpython.exeが～と表示された場合、python.exeにパスを通して下さい。  
やり方は[http://www.pythonweb.jp/install/setup/index1.html](http://www.pythonweb.jp/install/setup/index1.html)に載っています。

ちなみにnixysaとは純粋なC++コードをNPAPI用のコードに変換してくれるツールです。  
この変換にpythonが使われています。  

●ソースコードのダウンロード
----------------------------
downloads_srcをダウンロードして下さい。  
このGitHubページのZIPのとこからダウンロード出来たと思います。  

Google Driveからでもダウンロードできます。  
[https://docs.google.com/folder/d/0BxMqBbrrBxIDcVV6Q2h2SzJrQlk/edit?usp=sharing&pli=1](https://docs.google.com/folder/d/0BxMqBbrrBxIDcVV6Q2h2SzJrQlk/edit?usp=sharing&pli=1)  

「 **downloads_src.zip** 」と「 **downloads_src_without_nixysa.zip** 」  
の２つがありますが通常は「 **downloads_src.zip** 」を利用してください。  

「 **downloads_src_without_nixysa.zip** 」は既にnixysaを持っている人向けです。  
こちらをダウンロードした人は  
nixysa-read-only(任意のフォルダ名)  
┣　nixysa  
┣　third_party  
┣　tools  
┗　mycodes(任意のフォルダ名)  
　　　　┗　 **downloads** 　┳　downloads.h  
　　　　　　　　　　　　　 ┣　downloads.idl  
　　　　　　　　　　　　　 ┣　　　…  
の様に配置して下さい。  

●コンパイル
------------
downloads_src\examples\downloads\downloads.slnを開きます。  
(「 **downloads_src_without_nixysa.zip** 」をダウンロードした人はnixysa-read-only\examples\downloads\downloads.slnです）  

最初にツール＞設定＞上級者用の設定をチェックします。(この方が分かりやすいと思います。)  

上部ツールバー2段目真ん中あたりのソリューション構成が「 **Debug** 」になっていたら「 **Release** 」に変更する。  

32bit版Windowsを使ってる場合は
ツールバーのプロジェクト＞downloadsのプロパティ＞リンカー＞入力＞追加の依存ファイル  
の「 **C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib\WS2_32.Lib** 」を  
「 **C:\Program Files\Microsoft SDKs\Windows\v7.0A\Lib\WS2_32.Lib** 」に変更してください。  
たぶん無いと思いますが、「 **WS2_32.Lib** 」が別の場所にある場合はその場所を指定してください。

ツールバーのビルド＞ソリューションのビルドでコンパイルできます。  
Release\npdownloads.dllが生成されているはずです。  

またdownloads_src_without_nixysa.zipをダウンロードした人は  
[http://d.hatena.ne.jp/ichhi/20110306/1299434439](http://d.hatena.ne.jp/ichhi/20110306/1299434439)  
ここの「 **6.1 ビルドエラーの対処方法** 」を見てください。  


Microsoft Virtual PCにて上記の流れでコンパイル可能なことを確認しました。  
（[http://www.microsoft.com/en-us/download/details.aspx?id=11575](http://www.microsoft.com/en-us/download/details.aspx?id=11575)のWindows_7_IE8）

●NPAPIの自作の仕方
-------------------
downloads.hのdownloadsクラスの中身を編集して、それに合わせてdownloads.idlのメソッド定義部分を編集すれば  
すぐにオリジナルのNPAPIを作ることが出来ます。  
クラス名の変更はdownloads.h及びdownloads.idlのclass Downloadsの部分を変更すれば出来ます。  
プラグイン名の変更は…downloads.(def|h|idl|rc|slnなど)のファイル名を変更して  
これらのファイルとplugin.cc、SConstructの中のdownloads及びDownloadsとなってる部分をエディタ等で一括置換すれば出来ます。  
かなり無理やりだけど…。  

●メモ
--------
プラグインを作るのに読んだ解説サイト一覧  

～NPAPI～  

・覚え書き　NPAPIプラグインの作り方  
[http://masamitsu-murase.blogspot.jp/2011/11/npapi-for-windows-1.html](http://masamitsu-murase.blogspot.jp/2011/11/npapi-for-windows-1.html)  

・おし、プログラミング　Windowsでnixysaのサンプルプラグインをビルド  
[http://d.hatena.ne.jp/ichhi/20110302/1299073960](http://d.hatena.ne.jp/ichhi/20110302/1299073960)  

～C++～  

・太田研究室 > Visual C++ 2010 Express インストール メモ  
[http://cvwww.ee.ous.ac.jp/vc10inst.html](http://cvwww.ee.ous.ac.jp/vc10inst.html)  

・C++入門  
[http://www.asahi-net.or.jp/~yf8k-kbys/newcpp0.html](http://www.asahi-net.or.jp/~yf8k-kbys/newcpp0.html)  

・目指せプログラマー！  
[http://www5c.biglobe.ne.jp/~ecb/index.html](http://www5c.biglobe.ne.jp/~ecb/index.html)  

・Geekなぺーじ:winsockプログラミング  
[http://www.geekpage.jp/programming/winsock/](http://www.geekpage.jp/programming/winsock/)  

・winsockを使ってみる。(1) - while( c++ );  
[http://d.hatena.ne.jp/setuna-kanata/20100131/1264946168](http://d.hatena.ne.jp/setuna-kanata/20100131/1264946168)  

