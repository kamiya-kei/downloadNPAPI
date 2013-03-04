#include <stdio.h>
#include <winsock2.h>
#include <shlwapi.h>
#include <Windows.h>
#include <CommDlg.h>

#include <shlobj.h>

#include <vector>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include <process.h>

//using namespace std;

#pragma comment(lib, "shlwapi.lib" )
#pragma comment(lib,"shell32.lib")
#pragma once
#define BUF_SIZE 256

//デフォルト引数はjavascriptからの呼び出しでは使用不可

//宣言
class Downloads;

//スレッドのパラメーター定義
typedef struct _param{
	Downloads *ins;
	char *domain;
	char *rqpath;
	int port;
	std::string fullpath;
} param;

//プラグイン名もクラス名もサンプルまんま。後で変える。
class Downloads {
public:
	// nyxysaで複数class書く方法が分からない
	Downloads(){
		//コンストラクタ
		setFileSize(-1);
		setNowSize(0);
		setNowPer(0);
		setCancelFlag(0);
		setReferer("");
		setERROR("");
	}
	std::string post(std::string url, std::string postmsg){
		/**
		 * URLにPOSTしてCOOKIEを取得
		 * postmsg: ex) pixiv_id=(id)&pass=(pass)&mode=login&skip=1
		 */
		std::string cookie;
		char *domain;
		char *rqpath;
		int port;
		std::string p_domain;
		std::string _domain;
		std::string _path;

		WSADATA wd;
		SOCKET sock;
		struct sockaddr_in server;
		char buf[BUF_SIZE];
		unsigned int **addrptr;

		std::string fullpath;
		std::string tmpfile;

		int n= url.find("://");
		if (n == -1) {
    		std::cout << "not found ://" << std::endl;
    		return "";
		}
		p_domain = url.substr(n+3);
		n = p_domain.find("/");
		if (n == -1) {
    		std::cout << "not found /" << std::endl;
    		return "";
		}
		_domain = p_domain.substr(0, n);
		_path = p_domain.substr(n);
		rqpath = const_cast<char*>(_path.c_str());
		n = _domain.find(":");
		if (n == -1) {
    		domain = const_cast<char*>(_domain.c_str());
    		port = 80;
		} else {
    		std::string tmp = _domain.substr(0, n);
    		domain = const_cast<char*>(tmp.c_str());
    		port = atoi(_domain.substr(n + 1).c_str());
		}

		if ( WSAStartup(MAKEWORD(2,0), &wd) ) {
    		std::cout << "WSAStartup failed\n" <<  std::endl;
    		return "";
		}

		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET) {
    		std::cout << "socket : " << WSAGetLastError() << std::endl;
			WSACleanup();
    		return "";
		}

		server.sin_family = AF_INET;
		server.sin_port = htons(port);
		server.sin_addr.S_un.S_addr = inet_addr(domain);
		if (server.sin_addr.S_un.S_addr == 0xffffffff) {
      		struct hostent *host;
    		host = gethostbyname(domain);
    		if (host == NULL) {
    			if (WSAGetLastError() == WSAHOST_NOT_FOUND) {
    				std::cout << "host not found : " << domain << std::endl;
    			}else {
    				std::cout << "gethostbyname failed" << std::endl;
    			}
				closesocket(sock);
				WSACleanup();
    			return "";
    		}
    		addrptr = (unsigned int **)host->h_addr_list;
    		while (*addrptr != NULL) {
    			server.sin_addr.S_un.S_addr = *(*addrptr);
    			if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0) {
    				break;
    			}
    			addrptr++;
    		}
    		if (*addrptr == NULL) {
    			std::cout << "connect : " << WSAGetLastError() << std::endl;
				closesocket(sock);
				WSACleanup();
    			return "";
    		}
		}

		connect(sock, (struct sockaddr *)&server, sizeof(server));

		memset(buf, 0, sizeof(buf));
		sprintf_s(buf, sizeof(buf), "POST %s HTTP/1.1\r\n", rqpath);//メソッドをPOSTに
		send(sock, buf, (int)strlen(buf), 0);
		sprintf_s(buf, sizeof(buf), "host: %s\r\n", domain);
		send(sock, buf, (int)strlen(buf), 0);
		sprintf_s(buf, sizeof(buf), "\r\n");
		send(sock, buf, (int)strlen(buf), 0);
		sprintf_s(buf, sizeof(buf), postmsg.c_str());//空行の後にポストメッセージを送信
		send(sock, buf, (int)strlen(buf), 0);

		std::string source = "";
		while (1) {
    		char c;
    		n = recv(sock, &c, 1, 0);
    		if (n == SOCKET_ERROR) {
    			std::cout << "recv : " << WSAGetLastError() << std::endl;
				closesocket(sock);
				WSACleanup();
    			return "";
    		}
    		if (c == '\n'){
    			if (source.size() < 2) {
	   				break;
    			} 
    			std::cout << source << std::endl;
				std::string::size_type k1, k2;
				k1 = source.find("Set-Cookie: ");//クッキーの取得
				k2 = source.find(";");
    			if (k1 != -1){
					closesocket(sock);
					WSACleanup();
 					cookie = source.substr(k1 + 12, k2 - (k1 + 12));
					std::cout << cookie <<std::endl;
					addCookie(cookie);
   					return cookie;//成功
    			}
    			std::cout << "--------------------" << std::endl;
    			source = "";
    			continue;
    		}
    		source += c;
		}
		//失敗
		closesocket(sock);
		WSACleanup();
		return "";
	}

	//ダウンロード関連
	void download(std::string url, std::string path, std::string filename, int ask=0){
		/**
		 * ダウンロード
		 *
		 * filenameを空文字にしてpathにフルパスでもOK
		 * ask=1で名前を付けて保存ダイアログを表示
		 * 　その際、path, filenameは保存ダイアログでデフォルトパス、デフォルトファイル名として利用
		 *
		 * セキュリティ対策：
		 *   ・上書き保存時は確認ダイアログを出す
		 *   ・危険なパス(スタートアップフォルダなど)への保存は不可
		 */
		std::string domain, rqpath, fullpath;
		int port;
		std::string _domain;

		if (ask == 1) {
			//保存先をユーザーに確認する場合
			fullpath = saveFileDialog(path, filename);
			if (path.empty()) {
				std::cout << "ダウンロードはキャンセルされました" << std::endl;
				return;
			}
			std::string::size_type n = fullpath.find_last_of("\\");
			path = fullpath.substr(0, n);
			filename = fullpath.substr(n + 1, fullpath.size() - (n + 1));
		} else {
			//保存先を決まってる場合
			if (filename.empty()){
				//pathにフルパスを渡し、filenameを省略した場合
				fullpath = path;
				std::string::size_type n = fullpath.find_last_of("\\");
				if (n == std::string::npos) {
					n = fullpath.find_last_of("/");
					if (n == std::string::npos) {
						std::cout << "引数の値が不正です" << std::endl;
						return;
					}
				}
				path = fullpath.substr(0, n);
				filename = fullpath.substr(n + 1, fullpath.size() - (n + 1));
			} else {
				//pathとfilenameを普通に指定した場合
				fullpath = path + "\\" + filename;
			}

			//上書きチェック
			if (fileExist(path, filename)) {
				//上書きになる場合は確認ダイアログを出す
				std::cout << "ファイルが重複しています" << std::endl;
				int n = askDialog(fullpath + "を上書きしますか？", "確認", 0);
				if (n < 0) {
					//「キャンセル」選択でダウンロード中止
					std::cout << "ダウンロードはキャンセルされました" << std::endl;
					return;
				}else if (n == 0) {
					//「いいえ」選択は別の名前で保存(名前を付けて保存ダイアログ呼び出し)
					fullpath =saveFileDialog(path, getNonFilename(path, filename));
					if (fullpath.empty()) {
						//名前を付けて保存ダイアログで「キャンセル」時(ダウンロードキャンセル)
						std::cout << "ダウンロードはキャンセルされました" << std::endl;
						return;
					}
				}
			}
		}

		//保存先の安全性を確認
		if (securityCheck(path) == 0) {
			std::cout << "保存先が危険なパスです" << std::endl;
			return;
		}
		//保存先、ファイル名決め　ここまで
		std::cout << "fullpath: " << fullpath << std::endl;

		//URL解析(プロトコル、ドメイン、パス、ポートの取得)
		int n= url.find("://");
		if (n == -1) {
    		std::cout << "not found ://" << std::endl;
    		return;
		}
		_domain = url.substr(n+3);
		n = _domain.find("/");
		if (n == -1) {
    		std::cout << "not found /" << std::endl;
    		return;
		}
		domain = _domain.substr(0, n);
		rqpath = _domain.substr(n);
		n = domain.find(":");
		if (n == -1) {
			//URLにポート番号が含まれてない場合(ポート80)
    		port = 80;
		} else {
			//URLにポート番号が含まれてる場合
    		domain = domain.substr(0, n);
    		port = atoi(_domain.substr(n + 1).c_str());
		}
		//URL解析ここまで
		std::cout << "domain: " << domain << ", rqpath: " << rqpath << ", port: " << port << std::endl;

		//スレッドのパラメータの設定
		param p = { this, const_cast<char*>(domain.c_str()), const_cast<char*>(rqpath.c_str()), port, fullpath};
		//スレッドの起動
		h = (HANDLE)_beginthread(downloading, 0, &p);

		//これが無いとクラッシュする
		//でもこれ書くとマルチスレッドにならない…
		WaitForSingleObject(h, INFINITE);
		CloseHandle(h);
		return;
	}
	int getFileSize(){ return fileSize; }
	int getNowSize(){ return nowSize; }
	float getNowPer(){
		if (nowPer > 1.0) return 2.0;
		if (fileSize == -1.0) return -1.0;
		return (float)nowSize / fileSize;
	}
	int getCancelFlag(){ return cancelFlag; }
	std::string getCookie(){
		std::string ret = "";
		int n = cookies.size();
		for(int i = 0; i < n ; i++){
			ret += cookies[i] + ";";
		}
		return ret;
	}
	std::string getReferer(){ return referer; }
	std::string getERROR(){ return error; }
	void setFileSize(int value){ fileSize = value; }
	void setNowSize(int value){ nowSize = value; }
	void setNowPer(float value){ nowPer = value; }
	void setCancelFlag(int value){ cancelFlag = value; }
	void addCookie(std::string value){ cookies.push_back(value); }
	void clearCookie(){ cookies.clear(); }
	void setCookie(std::string value){
		LPSTR lp = NULL;
		clearCookie();
		char *fragment = strtok_s(const_cast<char*>(value.c_str()), ";", &lp);
		while(fragment!= NULL){
			addCookie(fragment);
			fragment = strtok_s(NULL, "\\", &lp);
		}
	}
	void setReferer(std::string value){ referer = value; }
	void setERROR(std::string value){ error = value; }

	//ダイアログ関連
	std::string selectDirDialog() {
		/**
		 * ディレクトリ選択ダイアログの呼び出し
		 * ツリータイプ(設定用)
		 */
		BROWSEINFO bi;
		char folder[260];
		LPITEMIDLIST idl;
		LPMALLOC g_pMalloc;
		SHGetMalloc(&g_pMalloc);

		memset(&bi, 0, sizeof(BROWSEINFO));
		bi.hwndOwner = NULL;
		bi.pszDisplayName = folder;
		bi.lpszTitle = TEXT("フォルダを選択してください");
		bi.ulFlags = BIF_RETURNONLYFSDIRS;
		idl = SHBrowseForFolder(&bi);
		if(idl){
			int b = SHGetPathFromIDList(idl, folder);
			if(b){
				return (std::string)folder;
			}
		}
		g_pMalloc->Free(idl);
		return "";
	}
	std::string saveFileDialog(std::string path, std::string filename) {
		/**
		 * 名前をつけて保存ダイアログの呼び出し
		 * (上書き保存の確認あり)
		 *
		 * 戻り値：ファイルのフルパス, キャンセル時は空白文字
		 */
		static OPENFILENAME ofn;
		static TCHAR szFile[MAX_PATH];

		char *tmp = const_cast<char*>(filename.c_str());
		strncpy_s(szFile, sizeof(szFile), tmp, strlen(tmp));
	
		memset(&ofn, 0, sizeof(OPENFILENAME));
		//ダイアログの設定
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrInitialDir = const_cast<char*>(path.c_str());//初期フォルダ
		ofn.lpstrFile = szFile;//結果を受け取るバッファ、初期ファイル名
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = TEXT("名前を付けて保存");
		ofn.lpstrFilter = TEXT("すべてのファイル(*.*)");
		//ofn.lpstrDefExt = TEXT(".jpg");
		ofn.Flags = OFN_OVERWRITEPROMPT;//上書きする場合、確認ダイアログを出す

		//ダイアログの呼び出し
		if ( GetSaveFileName(&ofn) ){
			std::cout << "szFile : " << szFile << std::endl;
			return szFile;//char*型→string型キャストしてくれる
		} 
		//キャンセルボタンを押した場合
		std::cout << "miss" << std::endl;
		return "";
	}
	int alertDialog(std::string msg){
		/**
		 * 警告ダイアログの呼び出し
		 */
		return MessageBox(NULL, msg.c_str(), "警告", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
	}
	int askDialog(std::string msg, std::string title, int flag){
		/**
		 * 確認ダイアログ(はい、いいえ、キャンセル)の呼び出し
		 * 
		 * 引数: flag) 1 = はいorいいえ, 0 = はいorいいえorキャンセル
		 * 戻り値:1 = はい, 0 = いいえ, -1 = キャンセル
		 * 
		 * ex) 保存の確認 (ファイル名)は既に存在します。\n上書きしますか？
		 */
 
		int res, tmp;
		if (flag == 0) {
			tmp = MB_YESNOCANCEL | MB_ICONWARNING | MB_SETFOREGROUND;
		} else {
			tmp = MB_YESNO | MB_ICONWARNING | MB_SETFOREGROUND;
		}
		res = MessageBox(NULL, msg.c_str(), title.c_str(), tmp);

		switch(res){
		case IDYES:
			return 1;
		case IDNO:
			return 0;
		case IDCANCEL:
			return -1;
		default:
			return -2;
		}
	}

	//パス関連
	int dirExist(std::string path){
		/**
		 * 指定したパスのディレクトリが存在するか確認
		 */
		return PathIsDirectory(path.c_str());
	}
	int makeDirs(std::string path){
		/**
		 * 再帰的なディレクトリ生成
		 * 戻り値：0=既にディレクトリが存在, -1=生成失敗
		 */
		LPTSTR lp = NULL;
		char *fragment;
		std::vector<std::string> words;
		std::string tmp_dir;
		int words_size;

		if (dirExist(path)) {
			std::cout << "DIR EXIST" << std::endl;//既にある
			return 0;
		}

		if (CreateDirectory(path.c_str(), NULL)) {
			std::cout << "CREATE DIR" << std::endl;//普通に作れる
			return 1;
		}

		//再帰的なディレクトリ生成
		fragment = strtok_s(const_cast<char*>(path.c_str()), "\\", &lp);
		while(fragment!= NULL){
			words.push_back(fragment);
			fragment = strtok_s(NULL, "\\", &lp);
		}
		tmp_dir = words[0];
		words_size = words.size();
		for (int i = 1; i < words_size; i++) {
			tmp_dir += "\\" + words[i];
			if (dirExist(tmp_dir)) {
				continue;
			}
			if (CreateDirectory(tmp_dir.c_str(), NULL) == 0) {
				//たぶんドライブが存在しないとか書き込み不可な場所とか
				std::cout << "can't create dir : " << tmp_dir << std::endl;
				return -1;
			}
			std::cout << tmp_dir << std::endl;
		}

		return 1;
	}
	int fileExist(std::string path, std::string filename = "") {
		/**
		 * 指定したパスに指定したファイルが存在するか確認
		 * filenameを省略してpathにフルパスを渡してもOK
		 */
		if (filename != "") {
			path += "\\" + filename;
		}
		return PathFileExists(path.c_str());
	}
	std::string getNonFilename(std::string path, std::string filename){
		/**
		 * 重複しない(上書きにならない)ファイル名を返す
		 * 同名のファイルが存在する場合、末尾に(数字)を付加する
		 *   ex) filename(1).jpg
		 */
		std::vector<std::string> words;
		std::string fname_name = "";
		std::string fname_ext;
		std::string fname_new;
		char fname_add[5];//「(99)\0」の5文字(数字は2桁まで)
		int fname_num = 1;
		int path_size = path.size();

		//ファイルが存在しなければそのままのファイル名を返す
		if (path.find_last_of("\\") != path_size || path.find_last_of("/") != path_size) {
			//パスの末尾が「\」又は「/」で無い場合は追加
			path += "\\";
		}
		if(fileExist(path, filename) == 0){
			std::cout << "OK. same file don't exists." << std::endl;
			return filename;
		}

		//ファイル名と拡張子を調べる
		std::cout << filename << std::endl;
		std::string::size_type n = filename.find_last_of(".");
		if (n != std::string::npos) {
			fname_name = filename.substr(0, n);
			fname_ext = filename.substr(n + 1, filename.size() - (n) + 1);
		} else {
			std::cout << "拡張子が見つかりませんでした" << std::endl;
			return "";
		}

		while(1){
			//ファイル名が重複してるので(1)の様に末尾に数字を付加
			sprintf_s(fname_add, "(%d).", fname_num++);
			fname_new = fname_name + fname_add + fname_ext;
			if(fileExist(path, fname_new) == 0){
				break;
			}
		}
		return fname_new;//重複しないファイル名を返す
	}
	std::string getDesktop(){
		/**
		 * デスクトップパスを取得
		 */
		std::string ret;
		char Path[256];
		SHGetSpecialFolderPath(NULL, Path, CSIDL_DESKTOP, FALSE);
		ret = Path;//char*型 → string型キャスト
		return ret;
	}
	int securityCheck(std::string _path){
		/**
			* スタートアップフォルダ及びパスの通ったフォルダ(システムフォルダ等)に
			* ダウンロード出来ないようにして、セキュリティの強化を図る
			* 
			* 戻り値：1 => 安全なパス, 0 => 危険なパス
			*/
		char startup[MAX_PATH];
		char *path = const_cast<char*>(_path.c_str());
		char *p;
		LPTSTR lptszNextToken = NULL;
		char *env;
		char *fragment;
		size_t requiredSize;

		SHGetSpecialFolderPath( NULL, startup, CSIDL_STARTUP, FALSE );//個別ユーザーのスタートアップ
		//std::cout << startup << std::endl;
		p = strstr(path, startup);
		if (NULL != p) {
			std::cout << "Alert: startup directory" <<std::endl;
			alertDialog("スタートアップフォルダにファイルを保存しようとしました。\nダウンロードを中止します。");
			return 0;
		}

		SHGetSpecialFolderPath( NULL, startup, CSIDL_COMMON_STARTUP, FALSE );//全てのユーザーのスタートアップ
		//std::cout << startup << std::endl;
		p = strstr(path, startup);
		if (NULL != p) {
			std::cout << "Alert: startup directory(all users)" << std::endl;
			alertDialog("スタートアップフォルダ(All Users)にファイルを保存しようとしました。\nダウンロードを中止します。");
			return 0;
		}

		//環境変数PATHの取得
		getenv_s(&requiredSize, NULL, 0, "Path");
		env = (char*) malloc(requiredSize * sizeof(char));
		getenv_s(&requiredSize, env, requiredSize, "Path");

		//環境変数PATHのディレクトリを１つずつ調べる
		fragment = strtok_s(env, ";", &lptszNextToken);
		while(fragment!= NULL){
			//std::cout << fragment << std::endl;
			p = strstr(path, fragment);
			if (NULL != p) {
				std::cout << "Alert : system environment variable 「PATH」 directory" << std::endl;
				std::string msg = fragment;
				msg += "\nにファイルを保存しようとしました。\nダウンロードを中止します。";
				alertDialog(msg);
				free(env);
				return 0;
			}
			fragment = strtok_s(NULL, ";", &lptszNextToken);
		}
		free(env);
		return 1;
	}
	int openExplore(std::string path){
		/**
		 * エクスプローラーで指定したパスを開く
		 */
		//HINSTANCE n = ShellExecute(NULL, "explore", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
		return 1;
	}

	//ファイル操作関連
	//セキュリティ対策
	//ファイルの削除・リネームはjavascriptから直接呼び出せないように
	//「Complex.idl」ファイルには記述しない
	//(downloading関数から呼び出すのでprivateにはしない)
	static int deleteFile(std::string path, std::string filename = "") {
		/**
		 * ファイルの削除
		 * filenameを省略してpathにフルパスを渡してもOK
		 */
		if (!filename.empty()) {
			path += "\\" + filename;
		}
		return DeleteFile(path.c_str());
	}
	static int renameFile(std::string before, std::string after, std::string path=""){
		/**
		 * ファイルのリネーム
		 * before:リネーム前のファイル名, after:リネーム後のファイル名, path:ファイルのある場所
		 * また、pathを省略してbefore, afterにフルパスを渡してもOK
		 * (その際beforeとafterのディレクトリが違えばファイルの移動も行う)
		 */
		if (!path.empty()) {
			before = path + "\\" + before;
			after  = path + "\\" + after;
		}
		if (rename(before.c_str(), after.c_str()) == 0) {
			std::cout << before << "を" << after << "にリネーム成功" << std::endl;
			return 1;
		} else {
			std::cout << "リネーム失敗" << std::endl;
			return 0;
		}
	}
private:
	int fileSize;
	int nowSize;
	float nowPer;
	int cancelFlag;
	std::vector<std::string> cookies;
	std::string referer;
	HANDLE h;
	std::string error;

	//ダウンロードのスレッド
	static void downloading(void* vp){
		/**
		 * ダウンロード(マルチスレッド用に分離)
		 *   javascriptから呼び出し不可
		 */
		//パラメータから情報抜き出し
		param p = *((param*)vp);
		Downloads *ins = p.ins;
		char *domain = p.domain;
		char *rqpath = p.rqpath;
		int port = p.port;
		std::string fullpath = p.fullpath;
		std::string tmpfile = fullpath + ".download";

		WSADATA wd;
		SOCKET sock;
		struct sockaddr_in server;
		char buf[BUF_SIZE];
		unsigned int **addrptr;

		if ( WSAStartup(MAKEWORD(2,0), &wd) ) {//winsock初期化
    		//初期化失敗
    		std::cout << "WSAStartup failed\n" <<  std::endl;
			ins->setERROR("WSAStartup failed");
    		return;
		}

		sock = socket(AF_INET, SOCK_STREAM, 0);//ソケットの作成
		if (sock == INVALID_SOCKET) {
    		//作成失敗
    		std::cout << "socket : " << WSAGetLastError() << std::endl;
			ins->setERROR("socketmake failed ");
			WSACleanup();
    		return;
		}
		//接続先指定用構造体の準備
		server.sin_family = AF_INET;//インターネット(TCP, UDP, etc...)
		server.sin_port = htons(port);//httpポート80
		server.sin_addr.S_un.S_addr = inet_addr(domain);//URL設定(inet_addrは32bitバイナリへの変換)
		if (server.sin_addr.S_un.S_addr == 0xffffffff) {
    		//URLがIP表記じゃない場合(名前解決が必要な場合)
    		struct hostent *host;
    		host = gethostbyname(domain);//名前解決(→IP)
    		if (host == NULL) {
    			//解決失敗
    			if (WSAGetLastError() == WSAHOST_NOT_FOUND) {
    				std::cout << "host not found : " << domain << std::endl;
					ins->setERROR("host not found");
    			}else {
    				std::cout << "gethostbyname failed" << std::endl;
					ins->setERROR("gethostbyname failed");
    			}
				closesocket(sock);
				WSACleanup();
    			return;
    		}
    		addrptr = (unsigned int **)host->h_addr_list;
    		while (*addrptr != NULL) {
    			//1つの名前に複数のIPが関連付けられていることがあるので接続が成功するまで全て試す
    			server.sin_addr.S_un.S_addr = *(*addrptr);
    			if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0) {
    				break;//接続成功
    			}
    			addrptr++;
    		}
    		if (*addrptr == NULL) {
    			//全てのIPで接続が失敗
    			std::cout << "connect : " << WSAGetLastError() << std::endl;
				ins->setERROR("connect failed");
				closesocket(sock);
				WSACleanup();
    			return;
    		}
    		
		}

		//サーバーに接続
		connect(sock, (struct sockaddr *)&server, sizeof(server));

		//HTTPリクエスト送信
		memset(buf, 0, sizeof(buf));//buf初期化(ゼロクリア)
		sprintf_s(buf, sizeof(buf), "GET %s HTTP/1.1\r\n", rqpath);
		int n = send(sock, buf, (int)strlen(buf), 0);
		if (n < 0) {
    		//送信失敗
    		std::cout << "send ; " << WSAGetLastError() << std::endl;
			closesocket(sock);
			WSACleanup();
			ins->setERROR("http request send failed");
    		return;
		}
		sprintf_s(buf, sizeof(buf), "host: %s\r\n", domain);
		n = send(sock, buf, (int)strlen(buf), 0);
		if (n < 0) {
    		std::cout << "send ; " << WSAGetLastError() << std::endl;
			closesocket(sock);
			WSACleanup();
			ins->setERROR("http request send failed");
			return;
		}
		if (!ins->getCookie().empty()) {
			//クッキーの設定
			std::string cookie = "Cookie: " + ins->getCookie() + "\r\n";
			std::cout << cookie << std::endl;
			sprintf_s(buf, sizeof(buf), cookie.c_str());
			send(sock, buf, (int)strlen(buf), 0);

		}
		if (!ins->getReferer().empty()) {
			//リファラの設定
			std::string ref = "Referer: " + ins->getReferer() + "\r\n";
			sprintf_s(buf, sizeof(buf), ref.c_str());
			send(sock, buf, (int)strlen(buf), 0);
		}
		sprintf_s(buf, sizeof(buf), "\r\n");
		n = send(sock, buf, (int)strlen(buf), 0);
		if (n < 0) {
		std::cout << "send ; " << WSAGetLastError() << std::endl;
			closesocket(sock);
			WSACleanup();
			ins->setERROR("http request send failed");
    		return;
		}

		//サーバーからデータ受信(ヘッダ)
		std::string source = "";
		while (1) {
    		//cに1文字ずつ取得してsourceに追記して改行が来たらsourceクリア
    		char c;
    		n = recv(sock, &c, 1, 0);
    		if (n == SOCKET_ERROR) {
    			//受信失敗
    			std::cout << "recv : " << WSAGetLastError() << std::endl;
				closesocket(sock);
				WSACleanup();
				ins->setERROR("recv failed");
    			return;
    		}
    		if (c == '\n'){
    			//1行毎の処理(sourceに1行のデータが格納されている)
    			if (source.size() < 2) {
					//ヘッダと実データの間は空行
					//でもsource.empty()じゃダメ。サイズは1(NULL文字'\0'が入ってるから？)
    				break;//ヘッダ終了
    			}
				//ステータス取得
				std::string::size_type k = source.find("HTTP/1.1 ");
				if (k != std::string::npos) {
					std::string status = source.substr(k + 9, source.size() - (k + 9));	
					int _status = atoi(status.c_str());
					if (_status != 200) {
						//400 Bad Request
						//403 Forbidden
						//404 Not Foundなどなど
						std::cout << "Status: " << status << std::endl;
						closesocket(sock);
						WSACleanup();
						ins->setERROR(status);
						return;
					}
				}
    			//ファイルサイズ取得
    			k = source.find("Content-Length: ");
				if (k != std::string::npos) {
    				std::string _fsize = source.substr(k + 16);
					ins->setFileSize(atoi(_fsize.c_str()));
					std::cout << "filesize: " << ins->getFileSize() << std::endl;
    			}
    			std::cout << source << std::endl;
    			std::cout << "------" << source.length() << "--------------" << std::endl;
    			source = "";//次の行を取得するためにクリア
    			continue;
    		}
    		source += c;//改行が来るまでsourceに蓄積
		}

		//サーバーからデータ受信(実データ)
		std::ofstream f;
		f.open(tmpfile, std::ios::out|std::ios::binary);//ダウンロード中は.downloadをファイル名末尾に付加
		//【メモ】
		while (1) {
    		memset(buf, 0, sizeof(buf));
    		n = recv(sock, buf, sizeof(buf) - 1, 0);//NULLを後で追加する分1バイト小さくする
			std::cout << ".";
    		if (n < 0) {
    			std::cout << "recv : " << WSAGetLastError() << std::endl;
				f.close();
				ins->deleteFile(tmpfile);
				closesocket(sock);
				WSACleanup();
				ins->setERROR("recv failed");
    			return;
    		}  		
			buf[n] = '\0';//NULL文字付加
    		f.write(buf, n);
			if (ins->getCancelFlag() == 1) {
				std::cout << "ユーザーによりキャンセルされました" << std::endl;
				break;
			}
			ins->setNowSize(ins->getNowSize() + n);//nowSize += n;
			if (n <= 0 || ins->getNowSize() >= ins->getFileSize()) {
				//受信サイズが0又は
				//ダウンロードサイズがファイルサイズに到達したら終了
				std::cout << "\nn(<=0) : " << n << std::endl;
    			break;
    		}
		}
		std::cout << "now size(end) : " << ins->getNowSize() << std::endl;
		std::cout << "file size     : " << ins->getFileSize() << std::endl;
		f.close();
		closesocket(sock);//ソケットを閉じる
		WSACleanup();//winsock終了
		if (ins->getCancelFlag() == 1){
			//キャンセル時
			ins->deleteFile(tmpfile, "");
			ins->setERROR("cancel");
			return;
		}
 		//ダウンロード完了
		ins->setNowPer(2.0);
		if(ins->fileExist(fullpath, "")) {
			//上書きするならファイル削除しとかないとリネーム失敗する
			ins->deleteFile(fullpath, "");
		}
		ins->renameFile(tmpfile, fullpath, "");//「.download」除去
		return;
	}
};


