= バ美肉おじさんになるための裏声を用いた統計的声質変換技術の基礎的検討
@<author>{Sython}
//profile{
D進はいいぞ。

こじらせてはいないので、抱き枕カバーは買っていません。
//}

//lead{
なれる！美少女バーチャルYouTuber
//}

== はじめに
オタク界隈における近年のバーチャルYouTuber（以降、VTuberと表記）の流行は目覚ましいものがあり、
2018年5月28日の時点でのVTuberの総数は3,000人を突破したと言われています@<fn>{moguravr}。
//footnote[moguravr][@<href>{https://www.moguravr.com/vtuber-3000/}]
最近では、VTuberを媒体として@<b>{バ}ーチャルな@<b>{美}少女に受@<b>{肉}する@<b>{「バ美肉」}がブームとなっており、
実際にバ美肉に成功したおじさん@<fn>{ojisan}は@<b>{「バ美肉おじさん」}と呼ばれています。
本稿では、外見（2Dまたは3Dアバター）だけでなく、声までも美少女化したいという欲張りなバ美肉おじさんの願望を叶えるための技術を提案します。
//footnote[ojisan][年齢的に実際におじさんかどうかは不明であることが多いですが、本稿では一貫して「おじさん」と称することにします。]

== バ美肉おじさんの種別
VTuberがここまで流行している背景には、人々の「自己表現の拡張」及び「自己承認欲求の解消」という2つの欲求があると考えられます。
「現実の世界に疲れたおじさんが美少女のアバターの皮を被り、インターネットの世界でチヤホヤされる」という行為は、
これら2つの欲求を同時に満たす非常に合理的な手段であると言えます。
本節では、バ美肉おじさんの種別を、代表的なVTuberを例に挙げながら解説します。

=== 外見の美少女化を用いたバ美肉おじさん
バ美肉には、何よりもまず外見を美少女化する必要があります。
VTuberブームの先駆けとなった代表的なVTuberの一人であるねこます氏（通称：バーチャルのじゃロリ狐娘YouTuberおじさん）@<fn>{nekomasu}は、
ケモミミの3D美少女アバター（@<img>{nekomasu}）を被ったおじさんが地声@<fn>{jigoe}で配信をするというインパクトのあるコンテンツが注目され、
まさに上記のバ美肉おじさんの先駆けともなった存在と言えるでしょう。
//footnote[nekomasu][@<href>{https://www.youtube.com/channel/UCt8tmsv8kL9Nc1sxvCo9j4Q}]
//footnote[jigoe][同氏がボイスチェンジャー等で声の美少女化をしない理由として、（1）ツールや機材のセットアップ等に時間をとられたくない、 （2）ボイスチェンジャーで変換された音声の音質が一定水準に達しないとそもそも動画を見てもらえない、という2点が挙げられています。（参考: @<href>{https://www.youtube.com/watch?v=tTmZAtfHPVU&feature=youtu.be&t=701} 11:45より）]
//image[nekomasu][ねこます氏のVRアバター][scale=0.75]

=== 外見と声の美少女化を用いたバ美肉おじさん
完全なバ美肉には、外見だけでなく、声も美少女化する必要があります。
ここでは、声の美少女化のための
（1）音声認識と音声合成を用いた手法、
（2)ボイスチェンジャーを用いた手法、
及び
（3）統計的声質変換技術を用いた手法
の3つを紹介します。

: （1）音声認識と音声合成の利用
  「生声での発声→音声認識→VOICEROIDなどの音声合成ソフトでの発声」という方法で声の美少女化を実現します。
  仕組みが非常にシンプルであるという利点がありますが、
  音声認識・合成に伴う発話遅延や、
  認識誤りによる意図しない発話の生成などが生じるという欠点があります。
  代表的なVTuberとしては、のらきゃっと氏（通称：バーチャルサキュバスおじさん、@<img>{noracat}）@<fn>{noracat}が挙げられます。
//footnote[noracat][@<href>{https://www.youtube.com/channel/UC3iwL9Yz8LcKkJsnLPevOTQ}]
//image[noracat][のらきゃっと氏のVRアバター][scale=0.3]

: （2）ボイスチェンジャーの利用
  リアチェンvoice@<fn>{rcvoice}や恋声@<fn>{koigoe}といったボイスチェンジャーソフトを利用して声の美少女化を実現します。
  リアルタイムでの声質変換が可能ですが、十分な品質の音声を生成するためにはある程度の訓練が必要、と言われています@<fn>{koigoeusing}。
  代表的なVTuberとしては、魔王マグロナ氏（通称：最強のバ美肉おじさん、@<img>{magurona}）@<fn>{magurona}や、
  竹花ノート氏（通称：癒やし系聖母バ美肉おじさん、@<img>{takebana}）@<fn>{takebana}が挙げられます。
//footnote[rcvoice][@<href>{https://crimsontech.jp/works/rcvoice/}]
//footnote[koigoe][@<href>{http://www.geocities.jp/moe_koigoe/koigoe/koigoe.html}]
//footnote[koigoeusing][筆者も実際に恋声を使ってみたのですが、ある程度の音質で変換できるようになるまでに話者自身の訓練とソフトのパラメータチューニング等が必要な気がしました。]
//footnote[magurona][@<href>{https://www.youtube.com/channel/UCPf-EnX70UM7jqjKwhDmS8g}]
//footnote[takebana][@<href>{https://www.youtube.com/channel/UCuPrcSmgo6twlAQzwYPlSkQ}]
//image[magurona][魔王マグロナ氏のVRアバター][scale=0.7]
//image[takebana][竹花ノート氏のVRアバター][scale=0.4]

: （3）統計的声質変換技術の利用
  統計的機械学習の技術を用いて声の美少女化を実現します。
  頑張ってデータを収集して声質変換モデルを学習させれば、
  自分の声を好きな声優などの声に変換する技術が実現できます@<fn>{L0-3-vc}。
  声質変換技術がまだまだ世の中に知られていないということもあり、
  残念ながら声質変換技術を用いたバ美肉おじさんVTuberはいまだに存在しない@<fn>{vc-babiniku}のですが、
  ニコニコ動画では、ディープラーニング声質変換を用いたVTuber（？）の動画が投稿されており@<fn>{hiho}、
  男性話者から結月ゆかりへの声質変換技術を実現しています。
//footnote[L0-3-vc][例：男性大学院生の声を某有名女性声優のささやき声に変換する技術（L0 Vol.3 @<href>{https://sigcoww.org/book/l0-3/}）]
//footnote[vc-babiniku][筆者の調査不足かもしれない]
//footnote[hiho][@<href>{http://www.nicovideo.jp/watch/sm33527293}]

上記の3つが代表的な手法ですが、やはり（1）、（2）と比較すると、まだまだ（3）の統計的声質変換技術を用いた手法はそこまで浸透していない印象があります。
本稿では、筆者が大学の研究室で統計的音声合成・声質変換技術を研究しているということもあり、
バ美肉おじさんになりたいという皆様のための統計的声質変換技術の可能性を探りたいと思います。

== 統計的声質変換技術の概要
声質変換とは、入力された音声の言語的な情報（発話内容）を保持しながら、それ以外の情報（例えば、話者性）を変換する技術です。
特に、実際に変換を行う部分（声質変換モデル）を統計的機械学習の枠組みで構築するものを統計的声質変換と呼びます。
本節では、統計的声質変換技術の枠組みを簡単に説明します。

=== 学習データの用意
本稿では、声質変換モデルの構築に、変換元・変換先話者の同一発話内容を収録したパラレルコーパスを利用します@<fn>{nonparavc}。
今回の実験では、コーパスとして日本声優統計学会@<fn>{seiyu}により公開されている「声優統計コーパス」を利用し、
変換元話者を某大学の男子大学院生Sさん@<fn>{Ssan}、変換先話者を藤東知夏さん@<fn>{fujitou}の「通常」発話音声@<fn>{emospeech}としました。
//footnote[nonparavc][パラレルコーパスを用いない手法もあります（例： @<href>{http://sython.org/papers/ICASSP/saito18icassp_vc.pdf}）。]
//footnote[seiyu][@<href>{https://voice-statistics.github.io/}]
//footnote[Ssan][SさんにはL0 Vol.3でもお世話になりました。]
//footnote[fujitou][@<href>{http://supershark.jp/talent_fujito.html}]
//footnote[emospeech][通常発話以外にも「喜び」、「怒り」の発話があります。]

=== 音声特徴量の抽出
人間の音声は、声帯振動によって生成された音源が、口、鼻、喉といった声道のフィルタを通ることによって作り出されています（@<img>{speech_process}）。
統計的声質変換では、音声波形から音源の特徴量と声道の特徴量を分析・抽出し、得られた音声特徴量のドメインで話者性を変換するモデルを学習します@<fn>{vq-vae}。
今回の実験では、音声分析に山梨大学の森勢先生によって開発されたWORLD@<fn>{world}（のPythonラッパーであるpyworld@<fn>{pyworld}）を利用し、
声帯振動の特性を基本周波数(F0)と非周期性指標(AP: APeriodicity)、
声道フィルタの特性をスペクトル包絡の特徴量（メルケプストラム係数）として抽出しました。
WORLDスペクトル包絡からメルケプストラム係数への変換には、音声信号処理ツールキットSPTK@<fn>{sptk}（のPythonラッパーであるpysptk@<fn>{pysptk}）を利用しました。
@<list>{analysis}に音声特徴量抽出のPythonコードを示します。
//image[speech_process][音声の生成過程：ソース・フィルタモデル（東京大学 信号処理特論第7回講義資料より引用）][scale=0.8]
//list[analysis][pyworldとpysptkを用いた音声特徴量抽出]{
from scipy.io import wavfile
import pyworld
import pysptk
import numpy as np

# load wav file (fs: sampling frequency)
fs, wav = wavfile.read('gomi.wav')
wav = wav.astype(np.float64)

# analysis parameters
#  alpha: warping coefficient
#  order: order of mel-cepstral coefficients
alpha = pysptk.util.mcepalpha(fs)
order = 39

# speech analysis
f0, sp, ap = pyworld.wav2world(wav, fs)
mcep = pysptk.sp2mc(sp, order=mcep_order, alpha=alpha)
//}
//footnote[vq-vae][最近では音声波形のドメインで直接変換するような手法も登場しています（@<href>{https://avdnoord.github.io/homepage/vqvae/}）が、リソースの問題上、これを再現するのは厳しそうな気がします。]
//footnote[world][@<href>{http://www.kki.yamanashi.ac.jp/~mmorise/world/index.html}]
//footnote[pyworld][@<href>{https://github.com/JeremyCCHsu/Python-Wrapper-for-World-Vocoder}]
//footnote[sptk][@<href>{http://sp-tk.sourceforge.net/}]
//footnote[pysptk][@<href>{https://github.com/r9y9/pysptk}]

=== 声質変換モデルの学習
抽出された音声特徴量を変換するための声質変換モデルを学習します。
本稿では、話者性を表す上で特に重要なメルケプストラム係数を変換するモデルを構築します@<fn>{source_features}。
声質変換モデルのアーキテクチャは、L0 Vol.3と同様のinput-to-output highway network@<fn>{highway}（@<img>{highway}）を用いました。
ここで、同図中の
@<m>{\bm{x\}}は変換元話者のメルケプストラム係数、
@<m>{\bm{\hat y\}}は変換された後のメルケプストラム係数です。
@<m>{\bm{\hat y\}}は次式で推定されます。
//texequation{
\bm{\hat y} = \bm{x} + \bm{T} (\bm{x}) \circ \bm{G} (\bm{x})
//}
@<m>{\bm{G\}(\bm{\cdot\})}は差分スペクトル特徴量推定器@<fn>{g}であり、
入出力特徴量の差分@<m>{\bm{y\} - \bm{x\}}を推定する役割を持っています。
@<m>{\bm{T\}(\bm{\cdot)\}}はhighway networkのtransform gate@<fn>{tgate}であり、
入力特徴量@<m>{\bm{x\}}に応じて@<m>{\bm{G\}(\bm{\cdot\})}の推定結果に重み付けをする役割を持っています。
ざっくり説明すると、違いが大きい部分はちゃんと変換して、違いが小さい部分は入力された特徴量をそのまま出力すればいい、という考え方です。
@<m>{\bm{G\}(\bm{\cdot\})}と@<m>{\bm{T\}(\bm{\cdot\})}は、
推定結果として得られる@<m>{\bm{\hat y\}}と、
変換先話者の音声特徴量@<m>{\bm{y\}}の二乗誤差
//texequation{
L_{\rm MSE}(\bm{y}, \bm{\hat y}) = (\bm{\hat y} - \bm{y})^\top (\bm{\hat y} - \bm{y})
//}
を最小化するように学習されます。
//image[highway][Input-to-output highway networks を用いた声質変換][scale=0.8]
//footnote[source_features][F0は変換元・変換先話者の平均と分散を用いて線形変換されます。非周期性指標は、そもそも変換されないことが多いです。]
//footnote[highway][@<href>{https://www.jstage.jst.go.jp/article/transinf/E100.D/8/E100.D_2017EDL8034/_article}]
//footnote[g][本稿では3層のFeed-Forwardネットワークを用います。隠れ素子数は256、隠れ層の活性化関数はReLU、出力層の活性化関数はLinearです。]
//footnote[tgate][1層のFeed-Forwardネットワークとして表現されます。出力層の活性化関数はsigmoidです。]

本稿では、変換音声をさらに高品質化するために、近年提案されたGenerative Adversarial Network (GAN)@<fn>{gan}の枠組みに基づく声質変換モデルの学習（敵対的声質変換）@<fn>{ganvc}を採用します。
敵対的声質変換では、@<img>{ganvc}に示すように、変換された音声と人間の自然音声を識別する識別モデルと、
声質変換モデルを交互に学習させる手法です。
識別モデル@<fn>{d}は、次式で定義される識別損失を最小化するように学習されます。
//texequation{
L_{\rm D}(\bm{y}, \bm{\hat y}) = L_{\rm D,1}(\bm{y}) + L_{\rm D,0}(\bm{\hat y})
//}
//texequation{
L_{\rm D,1}(\bm{y}) = -\sum_{t} \log D(\bm{y}_t), \quad L_{\rm D,0}(\bm{\hat y}) = -\sum_{t} \log ( 1 - D(\bm{\hat y}_t) )
//}
ここで、@<m>{L_{\rm D,1\}(\bm{\cdot\})}と@<m>{L_{\rm D,0\}(\bm{\cdot\})}はそれぞれ自然音声と合成音声に対する損失です。
@<m>{L_{\rm D\}(\bm{\cdot\})}の最小化により、識別モデルは自然音声に対して1を、変換音声に対して0を出力するように学習されます。
一方で、声質変換モデルは、次式で定義される損失関数を最小化するように学習されます。
//texequation{
L_{\rm G}(\bm{y}, \bm{\hat y}) = L_{\rm MSE}(\bm{y}, \bm{\hat y}) + \omega_{\rm D} \mathbb{E}_{\bm{\hat y}} [ \frac{L_{\mathrm{MSE}}}{L_{\mathrm{ADV}}} ] L_{\rm ADV}(\bm{\hat y})
//}
この式の第2項@<m>{L_{\rm ADV\}(\bm{\hat y\}) = L_{\rm D,1\}(\bm{\hat y\})}は、識別モデルを騙す（すなわち、変換音声に対して1を出力させる）損失であり、
自然音声と変換音声の特徴量の分布の違いを最小化する効果があります。
故に、敵対的声質変換における声質変換モデルの学習は、
従来の二乗誤差最小化に、GANに由来する分布間距離最小化の正則化を加えたものとして解釈できます。
@<m>{\omega_{\rm D\}}は第2項に対する重みパラメータで、本稿では1.0として設定します。
@<m>{\mathbb{E\}_{\bm{\hat y\}\} [L_{\mathrm{MSE\}\}/L_{\mathrm{ADV\}\}]}
は、2つの損失関数のスケールを調整する役割を持っています。
敵対的声質変換の枠組みをわかりやすくまとめたものを@<img>{ganvc_wakaru}@<fn>{ganvc_wakaru}に示します。
//image[ganvc][Input-to-output highway networkを用いた敵対的声質変換][scale=0.8]
//image[ganvc_wakaru][敵対的声質変換の概略図][scale=0.9]
//footnote[gan][@<href>{https://arxiv.org/abs/1406.2661}]
//footnote[ganvc][@<href>{https://ieeexplore.ieee.org/document/8063435/}]
//footnote[d][本稿では3層のFeed-Forwardネットワークを用います。隠れ素子数は128、隠れ層の活性化関数はReLU、出力層の活性化関数はsigmoidです。]
//footnote[ganvc_wakaru][@<href>{https://logmi.jp/294756} より引用]

=== 変換音声波形の生成
本稿では、実際に変換音声波形を生成するための方式として、
（1）音声特徴量変換とWORLDボコーダの利用@<fn>{fcvc}と（2）差分スペクトル法の利用@<fn>{sdvc}の2つを紹介します。

: （1）WORLDボコーダによる波形生成
  実際に変換された音声特徴量を用いて、WORLDボコーダにより音声波形を生成します（@<img>{fcvc}）@<fn>{vc}。
  声道の特徴量（メルケプストラム係数）だけでなく音源の特徴量（F0）も変換できますが、
  ボコーダによる分析・合成時に音声の品質が劣化します。

: （2）差分スペクトル法による音声波形変換
  変換元の音声波形に対し、変換先と変換元の話者対での差分スペクトルフィルタを畳みこむことで音声を変換します（@<img>{sdvc}））@<fn>{vc}。
  （1）における波形生成時のボコーダ処理に起因する品質劣化を回避できますが、
  音源の特徴量（要するに声の高さ）は変換できません。

（1）のボコーダ処理による品質劣化の影響は大きく、音源特徴量の変換を行わない場合（例えば、同性間での声質変換）では、
（2）の方式で高品質な声質変換を実現できます。
//image[fcvc][特徴量変換とWORLDボコーダを用いた波形生成][scale=0.8]
//image[sdvc][差分スペクトルフィルタによる音声波形変換][scale=0.8]
//footnote[fcvc][@<href>{https://ieeexplore.ieee.org/document/4317579/}]
//footnote[sdvc][@<href>{https://www.isca-speech.org/archive/interspeech_2014/i14_2514.html}]
//footnote[vc][@<href>{http://sython.org/papers/SP/saito201701sp.pdf} から引用]

== 裏声を用いた差分スペクトル法に基づく統計的声質変換
@<img>{f0hist_normalS}及び@<img>{f0hist_fujitou}に示すように、
変換元話者と変換先話者の性別が異なる場合、F0の分布（ヒストグラム）は大きく異なるため、
差分スペクトル法による声質変換を直接適用することはできません。
変換元音声のF0を線形変換して生成した波形に対して差分スペクトル法を適用する、
という手法@<fn>{diffvc_f0}も考えられますが、
音声の分析・合成というプロセスを挟むことによって品質が劣化するという問題があります。
//subfigw[F0のヒストグラム]{
//image[f0hist_normalS][変換元話者（Sさん、地声）][scale=0.33]
//image[f0hist_fujitou][変換先話者（藤東さん）][scale=0.33]
//image[f0hist_uragoeS][変換元話者（Sさん、裏声）][scale=0.33]
//}
//footnote[diffvc_f0][@<href>{http://www.ieice.org/ken/paper/20160328Ib6Y/}]

本稿では、
@<b>{「変換元話者が裏声で発話することにより、変換先話者が女性であってもそれなりに女性らしい高さの声を出せる」}
という仮定に基づき、裏声の音声波形に対して差分スペクトル法を直接適用する手法を提案します。
具体的な手順は非常にシンプルで、
（1）変換元話者の男性が裏声でパラレルコーパスを収録、
（2）変換元話者の裏声から変換先話者への変換を（メルケプストラム係数のドメインで）行うモデルを学習、
（3）裏声音声に対し、学習済みのモデルから推定された差分スペクトルフィルタ@<fn>{diff_filter}を畳み込んで声質変換、
となります。
Sさんの裏声音声から抽出されたF0のヒストグラムを@<img>{f0hist_uragoeS}に示します。
やはり変換先話者の藤東さん（@<img>{f0hist_fujitou}）と比較すると、分布の形状は異なっていますが、
F0の範囲はそれっぽく近づいており、F0の変換をしなくても差分スペクトル法がそれなりに上手く動いてくれることが期待できます。
//footnote[diff_filter][声質変換モデルから推定された@<m>{\bm{\hat y\}}から入力音声特徴量@<m>{\bm{x\}}を引いたものをフィルタとします。]

== 音声サンプル
今回の実験で生成した音声サンプルを@<fn>{samples}で公開しています。
波形生成の方法として
（1）WORLDボコーダの利用と
（2）差分スペクトル法の2通りを試しました。
公開しているサンプルは以下のとおりです。

 * Source (Normal): 変換元話者（Sさん）の音声（地声）
 * Source (Falsetto): 変換元話者（Sさん）の音声（裏声）
 * Target: 変換元話者（藤東さん）の音声
 * Converted (Normal-MSE): 地声の変換音声（MSE最小化で学習）
 * Converted (Normal-GAN): 地声の変換音声（敵対的声質変換で学習）
 * Converted (Falsetto-MSE): 裏声の変換音声（MSE最小化で学習）
 * Converted (Falsetto-GAN): 裏声の変換音声（敵対的声質変換で学習）

筆者が聴いた感想として、
まず、波形生成方式によらず、地声・裏声の両方で、敵対的声質変換を採用することで品質が改善しています。
学習に90文しか使っていないことも考慮すると、なかなか良いものができたのではないかと思います。
また、（1）の中での比較では、裏声を用いた場合のほうが変換先の話者に近い音声を生成できているような気がします。
さらに、（2）の中での比較では、裏声を用いた場合のほうがより自然な（機械音っぽくない）音声を生成できているような気がします。
厳密な評価をやったわけではないのですが、まとめとして、
@<b>{「裏声の男性話者から女性話者への変換を行う手法は、それなりにうまく動作する」}
と言えそうです。
//footnote[samples][@<href>{http://sython.org/demo/uragoeVC/demo.html}]

== おわりに
本稿では、バ美肉（@<b>{バ}ーチャルな@<b>{美}少女に受@<b>{肉}）を目的としたVTuberおじさんのための声質変換技術として、
裏声を用いた差分スペクトル法の枠組みを提案し、それなりに上手く動いてくれることを確認しました。
今後の課題としては、以下が考えられます。

=== 裏声収録時のコスト削減などの工夫
裏声音声のサンプルを聴くとわかるのですが、地声の音声と比べるとやはり話者が辛そうな気がします。
地声での変換モデルを裏声に適用するなどの方法で、より少ない裏声データから変換モデルを構築する枠組みが必要かもしれません。
また、パラレルデータを収録する際に、リファレンスの音声と録音音声のF0を可視化するようにすると、
高品質なデータが収集できそうです。

=== リアルタイムでの声質変換の実現
今回の実験で用いたスクリプトは全てPythonで作成しており、また、変換モデルに深層ニューラルネットを用いていることも考えると、
リアルタイムでの声質変換は厳しそうな気がします。
この辺は、C++への移植や、ニューラルネットの蒸留技術などを用いればなんとかなるかもしれません。

=== 多対多声質変換への拡張
現状だと、変換元と変換先の話者対が固定されており、VTuberへの応用を考えると正直あまり面白みがないという印象です。
多人数の話者による発話データを用いた学習によって多対多での声質変換（任意の話者 to 任意の話者）が実現できれば、
より面白く、かつ実用的なVTuber向け声質変換技術が確立できる気がします。
