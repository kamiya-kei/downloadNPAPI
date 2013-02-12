Download NPAPI Plugin for chrome, Windows
=========================================

■ソースコードのコンパイル
--------------------------

使い方はreadme.mdを見てください。

●環境の構築
------------
・まずNPAPIはC++なのでC++コンパイラが必要です。  
**Visual C++ 2010 Express**を以下からダウンロードします。  
[http://www.microsoft.com/visualstudio/jpn/downloads#d-2010-express](http://www.microsoft.com/visualstudio/jpn/downloads#d-2010-express)  
試用は30日間でそれ以降も利用する場合は無料登録が必要です。  

・NPAPIを作るのにnixysaを利用しているのでpythonが必要です。  
**Python 2.6.6**を以下からダウンロードします。  
[http://www.python.org/getit/releases/2.6.6/](http://www.python.org/getit/releases/2.6.6/)  

pywin32モジュールを使うので以下から「**pywin32-216.win32-py2.6.exe**」をダウンロードします。  
[http://sourceforge.net/projects/pywin32/files/pywin32/Build216/](http://sourceforge.net/projects/pywin32/files/pywin32/Build216/)  

上記3つを全てインストールすれば環境は整った…はずです。  

ちなみにnixysaとは純粋なC++コードをNPAPI用のコードに変換してくれるツールです。  
この変換にpythonが使われています。  

●ソースコードのダウンロード
----------------------------
complex_srcをダウンロードして下さい。  
このGitHubページのZIPのとこからダウンロード出来たと思います。  

Google Driveからでもダウンロードできます。  
[https://docs.google.com/folder/d/0BxMqBbrrBxIDcVV6Q2h2SzJrQlk/edit?usp=sharing&pli=1](https://docs.google.com/folder/d/0BxMqBbrrBxIDcVV6Q2h2SzJrQlk/edit?usp=sharing&pli=1)  

「**complex_src.zip**」と「**complex_src_without_nixysa.zip**」  
の２つがありますが通常は「**complex_src.zip**」を利用してください。  

「**complex_src_without_nixysa.zip**」は既にnixysaを持っている人向けです。  
こちらをダウンロードした人は  
nixysa-read-only(任意のフォルダ名)  
┣　nixysa  
┣　third_party  
┣　tools  
┗　examples(任意のフォルダ名)  
　　　　┗　**complex**　┳　complex.h  
　　　　　　　　　　 ┣　complex.idl  
　　　　　　　　　　 ┣　　　…  
の様に配置して下さい。  

●コンパイル
------------
complex_src\examples\complex\complex.slnを開きます。  
(「**complex_src_without_nixysa.zip**」をダウンロードした人はnixysa-read-only\examples\complex\complex.slnです）  

ツールバーのビルド＞ソリューションのビルドでコンパイルできます。  
Release\npcomplex.dllが生成されているはずです。  

もしエラーが出た場合、  
ツールバーのプロジェクト＞complexのプロパティ＞リンカー＞入力＞追加の依存ファイル  
がWS2_32.Libへのフルパスが入ってることを確認してください。  
C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib\WS2_32.Libみたいな感じです。
(slnにこういう設定も含まれてるかな？)  

またwithout_nixysaをダウンロードした人は  
[http://d.hatena.ne.jp/ichhi/20110306/1299434439](http://d.hatena.ne.jp/ichhi/20110306/1299434439)  
ここの「**6.1 ビルドエラーの対処方法**」を見てください。  

●最後に
--------
一応プラグインを作るのに読んだ解説サイト一覧を載せておきます。  

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

