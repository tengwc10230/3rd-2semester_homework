#include<bits/stdc++.h>
using namespace std;

vector<string>word[20];
struct word_block{
	int x;
	int y;
	int len;
	string dir;
	int cross;
	friend bool operator < (const word_block& a,const word_block& b){
		return a.cross > b.cross;
		//return word[a.len].size() < word[b.len].size();
	}
};
char b[50][50]; //current board
int deg[50][50]; //count amount of cross
int mxx,mxy,ans_cnt,flag,node_cnt;
set<string>use;//word that already used in current dfs node
vector<word_block>wb; // every word block's content

void dfs(int now){
	node_cnt++;//visit new node
	if(flag)return;//if already find answer
	if(now==(int)wb.size()){// all word block already assinged then print
		cout<<endl;
		for(int i = 0 ; i < mxx+1 ; i++){
			cout<<' ';
			for(int j = 0 ; j < mxy+1 ; j++){
				cout<<b[j][i]<<' ';
			}
			cout<<endl;
		}
		flag = 1;//find answer
		cout<<endl;
		//system("PAUSE");
		//ans_cnt++;
		/*if(ans_cnt%10000 == 0){
			cout<<ans_cnt<<endl;
		}*/
		return;
	}
	int x=wb[now].x;
	int y=wb[now].y;
	int len=wb[now].len;
	string dir=wb[now].dir;
	int xs=0,ys=0;// x shift , y shift (dir)
	if(dir == "A"){
		xs = 1;
	}
	else{
		ys = 1;
	}
	for(auto i:word[len]){// iterate all word consist current needed length 
		if(use.find(i)!=use.end())continue; // already used
		int f=0;
		int nx=x,ny=y;
		for(int j=0;j<len;j++){
			if(b[nx][ny]!='_'&&b[nx][ny]!=i[j]){// check if ok
				f=1;
				break;
			}
			nx += xs;
			ny += ys;
		}
		nx=x,ny=y;
		if(!f){
			vector<pair<int,int> >sp;// memory assigned characters in this node
			nx=x;
			ny=y;
			for(int j=0;j<len;j++){
				if(b[nx][ny]=='_'){
					sp.push_back({nx,ny});
					b[nx][ny]=i[j];
				}
				nx+=xs;
				ny+=ys;
			}
			use.insert(i);
			dfs(now+1);
			use.erase(i);
			for(auto j:sp){// remove assigned characters in this node
				b[j.first][j.second]='_';
			}
		}
	}
}
int main(){
	ios_base::sync_with_stdio(0);
	cin.tie(0);
	clock_t start,stop;
	string s;
	fstream fin;
	fin.open("English words 3000.txt",ios::in);
	string add;
	while(fin >> add){
		word[add.length()].push_back(add);
	}
	while(getline(cin,s)){
		stringstream ss(s);// to read every word block
		int x,y,len; // start position and length
		string dir; // direction of word
		mxx=0;		// x y bound of this dataset
		mxy=0;		
		wb.clear();
		while(ss >> x >> y >> len >> dir){
			wb.push_back({x,y,len,dir,0});
			mxx=max(mxx,x+(dir=="A")*len);
			mxy=max(mxy,y+(dir=="D")*len);
			int px=x,py=y;//iterate every pos that this word contain
			while(len--){
				deg[px][py]++;
				if(dir=="A")px++;
				else py++;
			}
		}
		
		for(auto &i:wb){
			int px=i.x,py=i.y;
			for(int j=0;j<i.len ;j++){
				i.cross += deg[px][py] - 1; // count cross heuristic
				if(i.dir=="A")px++;
				else py++;
			}

		}
		sort(wb.begin(),wb.end());//operate heuristic
		for(int i = 0 ; i < mxx+1 ; i++){ //init board
			for(int j = 0 ; j < mxy + 1 ; j++){
				b[i][j]='_';
			}
		}
		ans_cnt = 0; // for counting amount of answer
		flag = 0; // alrady find ans?
		start = clock();
		dfs(0);
		stop = clock();
		use.clear();
		cout << double(stop - start) / CLOCKS_PER_SEC <<endl;
		cout << node_cnt << endl;
	}

}