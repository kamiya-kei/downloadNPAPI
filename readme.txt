Windows専用ダウンロードNPAPI for chrome 
Complex(仮) ver 0.0.0.1

作者：PekeTaichi
開発環境：windows7 64bit, Visual C++ Express, Google Chrome 26.0.1403.0 dev-m
日付：2013-02-11

まだちゃんとした動作チェックは出来ていないので
おそらく色々と問題があると思いますが…よかったら使って下さい。

●インスタンスの生成
html
<object id="pluginId" type="application/complex" style="width: 0; height: 0;">Plugin FAILED to load</object>
js
pluginelm = document.getElementById("pluginId");
c = pluginelm.Complex();

※以下、メソッドの引数で受け取るパスには末尾に「\」を付けないで下さい。

●ダウンロード
・ダウンロード
c.download(url, savepath, filename);
    url:URL
    savepath:保存先
    filename:ファイル名
    ※filenameを空白文字にしてsavepathにフルパス指定でも可

複数のファイルをダウンロードする場合は必ず下記のようにインスタンスを分けてください。
ダウンロード状況の確認、キャンセル等が出来なくなります。
var c1 = pluginelm.Complex();
var c2 = pluginelm.Complex();
c1.download(url1, savepath1, filename1);
c2.download(url2, savepath2, filename2);

上記メソッド実行後
・ダウンロードするファイルのサイズの取得
c.getFileSize();
    戻り値：ファイルのサイズ(ヘッダのContent-Length:～の部分)
・ダウンロード状況の確認(Bytes)
c.getNowSize();
    戻り値：ファイルのサイズ(256バイト毎に更新)
・ダウンロード状況の確認(％)
c.getNowPer();
    戻り値：ダウンロード状況をfloatで返す(上2つを割っただけ)
            ファイルサイズ未取得の場合は-1.0
            ダウンロード完了済みの場合は2.0を返す

・ダウンロードのキャンセル・ファイルの削除
c.setCancelFlag(1);

・用途不明
c.setFileSize(size);
c.setNowSize(size);
c.setNowPer(size);

c.getCancelFlag();
    戻り値：0or1(キャンセル済み)

●その他
・ディレクトリの存在確認
c.DirExist(path);
    path:ディレクトリのパス
    戻り値：1(有)or0(無)
    <単にPathIsDirectoryの戻り値を返す>

・ディレクトリの生成(再帰的)
c.makeDirs(path);
    path:生成したいディレクトリを含めたフルパス
    戻り値：1(成功)or0(失敗)

・ファイルの削除
c.deleteFile(path, filename);
    path:ファイルのパス
    filename:ファイル名
    ※filenameを空白文字にしてpathにフルパス指定でも可
    戻り値：1(成功)or0(失敗)
    <単にDeleteFileの戻り値を返す>

・ファイルの存在確認
c.FileExist(path, filename);
    path:ファイルが存在するディレクトリのパス
    filename:ファイル名
    ※filenameを空白文字にしてpathにフルパス指定でも可
    戻り値：1(成功)or0(失敗)
    <単にPathFileExistsの戻り値を返す>

・重複しないファイル名の取得
c.getNonFilename(path, filename);
    path:ファイルが存在するディレクトリのパス
    filename:ファイル名
    戻り値：ファイル名末尾に(1)や(2),(3)...のように既存ファイルと被らないファイル名を返す

・デスクトップのフルパスを取得
c.getDesktop();
    戻り値：デスクトップのフルパス
    <単にSHGetSpecialFolderPath(NULL, Path, CSIDL_DESKTOP, FALSE)のPathを返す>

・ディレクトリ選択ウィンドウの呼び出し
c.selectDir();
    戻り値：選択したディレクトリのフルパス
