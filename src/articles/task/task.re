= 始めよう、スケジューリング問題！
@<author>{かふぇした}
//profile{
    ねぇ無理無理！（社会人）

    こじらせてはいませんが、初音ミクシンフォニーのSS席を取りました。
//}

//lead{
    いやーキツいっす（社会人）
//}

== この記事のやりたいこと
  * @<b>{ジョブショップスケジューリング問題について解説します．}
  * @<b>{以上です．}

猛暑@<fn>{mosho}の中，皆さんはいかがお過ごしでしょうか．
私は今年から社会人になり，タスク管理の重要性を痛感しているところです．
//footnote[mosho][2018年8月現在]

タスク管理を定式化した組合せ最適化問題として，@<b>{スケジューリング問題}があげられます．
スケジューリング問題のなかでも，最も実用の例に近いとされる
@<b>{ジョブショップスケジューリング問題 (JSP; Job-shop Scheduling Problem)}は，
古くから研究の対象とされてきました．
JSPは，また，定義が単純かつ分かりやすい一方，厳密解法が判明していない困難な問題でもあるため，
@<b>{皆さん大好き巡回セールスマン問題}と共に，近似解法のベンチマークとしても広く利用されています．

今回は，JSPを含めたスケジューリング問題，およびその解法についてざぁっと説明します．
実装によるベンチマークは行いません@<fn>{zikan}．お兄さん許して！
//footnote[zikan][圧倒的に時間と気力が足りない]

== スケジューリング問題
スケジューリング問題は，処理されるべき@<b>{仕事}と，それを処理する@<b>{機械}によって構成されます．
仕事と機械はそれぞれ複数個存在し，各仕事は複数の機械によって処理されます．
@<img>{schedule}に，この状況を示します．
この例では，各仕事が機械A，機械B，機械Cの順で処理されます．
例えるなら，複数の鉄鋼品の製造（仕事A～C）を鋳造（機械A），圧延（機械B），加工（機械C）によって処理するようなイメージでしょうか．

//image[schedule][3機械3仕事のスケジューリング問題の例．][scale=0.8]

原則として，機械は，同時に複数の仕事を処理することはできません．また，仕事は，同時に複数の機械に処理されることはできません．
これらの制約を満たしたうえで，@<img>{schedule}の各仕事を処理する例を@<img>{make1}に示します．

//image[make1][@<img>{schedule}におけるガントチャートの例．][scale=1.0]

@<img>{make1}のように，縦軸に機械を取り，各機械が処理する仕事を時系列順に並べた図を@<b>{ガントチャート}と呼びます．
また，最初の仕事の開始時点（@<img>{make1}では機械Aが仕事Aの処理を開始する時点）から，最後の仕事の終了時点（@<img>{make1}では機械Bが仕事Cの処理を終了する時点）までの時間を@<b>{メイクスパン}
と呼びます．
スケジューリング問題は，このメイクスパンを最小にすることが主な目的になります．

@<img>{make1}は，何も考えず機械Aに仕事A，仕事B，仕事Cを投入し，各仕事が終わり次第，続く機械に仕事を投入しています．
しかし，（機械Aにとって）最も待ち時間が長い仕事Aを投入したばっかりに，機械Bと機械Cがかなり待たされています．
ここで，最も待ち時間が短い仕事Cを最初に投入した例を@<img>{make2}に示します．

//image[make2][@<img>{schedule}におけるガントチャートの例．@<img>{make1}よりもメイクスパンが短くなっていることに注目されたい．][scale=1.0]

@<img>{make2}を見ると，@<img>{make1}よりもメイクスパンが短くなっていることが分かります．
このように，単純な例でも仕事の処理手順によってメイクスパンが変化することが分かっていただけたかと思います．

=== スケジューリング問題の分類
スケジューリング問題は，機械の処理順序に対する制約により，次の3つに分類されます．

: オープンショップ問題 (OSP; Open-shop Scheduling Problem)
  仕事を処理する機械の順番が決まっていない問題を指します．@<img>{schedule}を例にとると，仕事Aを処理するのに，例えば機械C，機械B，機械Aの順番で処理して良いということになります．

: フローショップ問題 (FSP; Flow-shop Scheduling Problem)
  仕事を処理する機械の順番が決まっており，すべての仕事で処理順序が同じ問題を指します．@<img>{schedule}を例にとると，仕事Aを処理するためには，必ず機械A，機械B，機械Cの順番で処理しなければなりません．

: ジョブショップ問題 (JSP; Job-shop Scheduling Problem)
  仕事を処理する機械の順序が決まっており，各仕事において処理順序が違って良い問題を指します．@<img>{flowjob}に，FSPとJSPの違いを示します．

//image[flowjob][フローショップ問題とジョブショップ問題の比較．フローショップ問題ではすべての仕事の処理順序が同じであるのに対し，ジョブショップ問題ではこの制約が存在しない．][scale=0.8]

一般に，フローショップ問題よりもジョブショップ問題の方が困難であるとされています．
一方で，FSPをJSPの特殊例と考えれば，JSPの方がより実用に近いということができます．

=== スケジューリング問題の困難性
スケジューリング問題における困難性は，仕事と機械の数によって決まります．
仕事の数が@<m>{n}，機械の数が@<m>{m}であるようなスケジューリング問題を，@<m>{n}仕事@<m>{m}機械のスケジューリング問題といいます．
最も困難なJSPでも，@<m>{n}仕事@<m>{2}機械のJSPは，多項式時間による厳密解法が判明しています．
逆に，@<m>{n}仕事3機械のJSPはNP困難です@<fn>{hard}．@<m>{n, m}の値が大きくなるにつれ，指数関数的に困難になっていきます．
//footnote[hard][NP困難であることと，解くことが困難であることは別です．@<m>{n}仕事3機械であれば，@<m>{n}が小さい値であれば，後述する分枝限定法などで現実的な時間による厳密解の発見が可能です．]

=== その他の制約: 派生モデル
JSPは最も実用に近いと書きましたが，実用の場では様々な制約や派生が考えられます．

JSPにおける仕事の各工程において，複数の機械を選択可能とした拡張をフレキシブル・ジョブショップスケジューリング問題（F-JSP; Flexible JSB）と呼びます．
F-JSPはJSPの派生の中でも特に実用の問題に近いため，盛んに研究されている問題です．
F-JSPのように定式化された派生以外にも，
機械の起動・停止時間の制約@<fn>{machine1}，
段取り時間の制約@<fn>{machine2}，仕事ごとに決められた納期の遵守などによる派生が考えられます．
//footnote[machine1][@<img>{make2}において，機械の起動・停止に時間がかかる場合，機械Cのような細切れの作業はできないかもしれません．]
//footnote[machine2][段取り時間とは，同じ機械で違う仕事をするときに発生する取替時間のことを言います．たとえば，同じ機械で違う材料を処理するときに，内部の洗浄が必要とされる場面を想定しています．]

JSPを人間のタスク管理に応用したとき，すなわち機械を人として考えたときは，様々な制約が考えられます．
例えば，連続勤務時間の制約@<fn>{human1}，勤務時間帯希望の制約などが考えられます．
このような，人間特有の制約を考慮したスケジューリング問題をナーススケジューリング問題 (NSP; Nurse Scheduling Problem)とよびます．
//footnote[human1][@<img>{make2}の機械を人として捉えると，横軸のスケールによっては，機械Aと機械Bの人は過労死してしまうでしょう．]

== アルゴリズム
=== ジョンソン法 (Johnson's Algorithm)
ジョンソン法は，@<m>{n}仕事2機械のFSPを解く厳密解法です@<fn>{johnson}．
とっても有名なので知っている人もいるかと思います．
//footnote[johnson][このアルゴリズムが考案されたのは1954年．太古のアルゴリズムです．]

FSPはすべての仕事で処理順序が同じという制約があります．
2機械のFSPを解く際に，先に処理を行う機械を前工程機械，後に処理を行う機械を後工程機械としたとき，
ジョンソン法のアルゴリズムは以下のようになります．

1. すべての仕事-機械対のなかで，最も処理時間が短いものを選択する．
2. 選択した仕事-機械対が，前工程機械のものであれば最初に処理を行い，後工程機械のものであれば最後に処理を行う．
3. 選択した仕事をリストから削除する．
4. リストに仕事が残っているなら，1へ戻る．残っていなければ終了．

@<img>{john}に，ジョンソン法による例を示します．

//image[john][ジョンソン法による操作の例．まず，最も処理時間が短いJ3-M1対(3)が選択される．M1は前工程機械であるため，J3は最初に配置される．次に，残った仕事の中で最も処理時間が短いJ4-M2対(4)が選択される．M2は後工程機械であるため，J4は最後に配置される．図では省いたが，次に選択されるのはM1-J2対(6)であり，J2はJ3の後に配置される．よって，この例の解は\{J3,J2,J1,J4\}となる．][scale=1.0]

=== ジャクソン法 (Jackson's Algorithm)
ジャクソン法は，@<m>{n}仕事2機械のJSPを解く厳密解法です@<fn>{jackson}．
ジョンソン法の考え方をベースにしています．
//footnote[jackson][このアルゴリズムが考案されたのは1956年．太古のアルゴリズムです．]

JSPは，処理順序が仕事によって異なることに留意しなければなりません．
ジャクソン法では，2つの機械を@<m>{M1, M2}としたとき，各仕事を以下のグループに分けます．

 * @<m>{G_{M1}}: 機械@<m>{M1}にのみ処理されるジョブ
 * @<m>{G_{M2}}: 機械@<m>{M2}にのみ処理されるジョブ
 * @<m>{G_{M1 \to M2}}: 機械@<m>{M1}で処理された後，機械@<m>{M2}で処理されるジョブ
 * @<m>{G_{M2 \to M1}}: 機械@<m>{M2}で処理された後，機械@<m>{M1}で処理されるジョブ

グループ分けを行ったら，@<m>{G_{M1 \to M2}, G_{M2 \to M1}}それぞれに対し，ジョンソン法を適用します．
このとき，@<m>{G_{M2 \to M1}}では機械@<m>{M2}が前工程機械，@<m>{M1}が後工程機械になることに注意してください．
@<m>{G_{M1}, G_{M2}}に属するジョブは任意の順番で構いません．

最後に，機械@<m>{M1, M2}の処理順序を以下のように決定します．

 * 機械@<m>{M1}: @<m>{G_{M1 \to M2}, G_{M1}, G_{M2 \to M1}}の順で処理を行う．
 * 機械@<m>{M2}: @<m>{G_{M2 \to M1}, G_{M2}, G_{M1 \to M2}}の順で処理を行う．

=== 分枝限定法 (B&B; Branch and Bound)
B&Bは，組合せ最適化問題の厳密解を効率的に求めるための枠組みです．
比較的小規模なJSP@<fn>{BB}は，このB&Bによって現実的な時間内に厳密解を得ることが可能です．
以下で，B&Bの簡単な説明を行います@<fn>{suman1}．
//footnote[BB][大体10仕事10機械ぐらい．]
//footnote[suman1][時間と気力が足りない．]

B&Bでは@<b>{分枝操作}と@<b>{限定操作}を繰り返すことで，探索領域を絞り込み，効率的な探索を行います．

分枝操作では，実行可能領域@<fn>{const}を複数の部分領域に分割します．
それぞれの部分領域における最良の値を求めることで，実行可能領域全体の最適解を求めることを目的としています．
//footnote[const][考えうるすべての解候補が張る空間のこと．]

限定操作では，各部分領域の上界下界を調べ，最適解が見つかる見込みのない領域を以降探索しないようにします．
上界(UB; Upper Bound)とは，最適解の考えうる最悪の値を指します．通常は，それまでに見つかった最良解を用います@<fn>{ub}．
下界(LB; Lower Bound)とは，最適解の考えうる最良の値を指します．こちらは，任意の式によって解析的に求めます．
このとき，ある部分領域@<m>{S_A}の下限が，他の部分領域@<m>{S_B}の上限よりも悪いとき，@<m>{S_A}は探索の必要がないと判断します．
つまり，@<m>{S_A}で見つかる最良のケースが，@<m>{S_B}で見つかる最悪のケースよりも悪い時点で，@<m>{S_A}に最適解があるはずがないと判断するということです．
//footnote[ub][運が良ければその最良解が最適解です．そうでなくても，最適解は最良解よりも小さい（最大化問題なら大きい）ため，最良解=最適解の最悪のケースとなるわけです．]

B&Bでは主に，領域をどう分割するか，下界をどう算出するか，を設計者が決定する必要があります．

=== 遺伝的アルゴリズム (GA; Genetic Algorithm)
大規模なJSPは，B&Bでも解くことが難しくなります．
そこで，@<b>{満を持して}近似解法の登場です．
近似解法についての詳しい説明は，既刊@<b>{COSMIC L0 Vol.3}を読んでいただくとして，ここでは簡単な説明にとどめます@<fn>{suman2}．
//footnote[suman2][時間と気力が足りない．]

GAは，生物の進化過程を模倣した汎用的近似解法の枠組みです．
あくまでも解法の「枠組み」であるため，具体的な実装は設計者に委ねられますが，
問題固有の性質をうまく利用した設計を行えば，優れた性能を示すことが知られています．
例えば，@<b>{皆さん大好き巡回セールスマン問題}への適用例であるGA-EAXは他のソルバを凌駕する圧倒的な性能を示しています．

GAは，JSPに対しても適用されています．
小規模かつ簡単な問題はB&Bでも解けてしまうため，
GAの主なベンチマークは，ApplegateとCookらによって提案された「ten tough problems@<fn>{ten}（10の難問）」を対象としています．
//footnote[ten][言ってしまえば，その当時の研究者たちが，B&Bを始めとした種々のソルバを用いても解くことができなかった問題をまとめたものです．今では殆ど解かれてしまいましたが．]

GAによるJSPへの接近としては，論文[小野 98]@<fn>{ONO98}で提案された交叉法JOX(Job-based Order Crossover)があげられます．
JOXは，順序交叉という比較的単純な交叉オペレータをJSPに適用したもので，JSPの性質をうまく利用した，優れた交叉オペレータといえます．
JSPに対するGAの改良は，ほとんどがこのJOXを交叉法として利用している，あるいはベースとしています．
//footnote[ONO98][小野, 小林: Inter-Machine JOXに基づくJSPの進化的解法，人工知能学会誌，Vol. 13，No. 5，pp. 780-790 (1998)．]

なお，ten tough problemsは，Googleが公開しているor-toolsリポジトリ@<fn>{google}で入手可能です．
いくつか問題がありますが，abz7 ~ 9, la21, la24, la25, la27, la29, la38, la40が該当します．
ten tough problemsに関する最新の動向は未調査ですが，2009年時点でabz8, abz9が未解決で，それ以外は最適解の発見に成功しているようです．
//footnote[google][@<href>{https://github.com/google/or-tools/tree/master/examples/data/jobshop}]

== おわりに
今回はスケジューリング問題について説明し，それを解くためのアルゴリズムについてざっと説明しました．
また機会があれば，ソルバの詳しい説明と，実装によるベンチマークを行う...かもしれません@<fn>{theme}．
//footnote[theme][TSPみたいに，やっててモチベーションが上がる問題を解きたい．]
