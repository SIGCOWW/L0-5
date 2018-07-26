= おNICの照リーヌ DPDK仕立て
@<author>{スパッツクリックカタルシス！, lrks}
//profile{
こじらせてしまい、初任給で抱き枕カバーを追加購入してしまいました。
会長ごめんなさい。響ちゃんと皐月ちゃん可愛いです。
艦娘が戦意高揚した回数の見えるメガネ、ディープラーニングってやつでなんとかなりませんか。
//}

//lead{
本日、「すてきな同人誌」のお記事を担当させていただきます、出張Chefのlrksです。
じゃあさっそく準備のほうに入らせてもらいますので、よろしくお願いします。
//}

#@# Todo: 擬似和音
#@# Todo: レジスタ弄ったらステータスLEDも点けられないかなぁ

== はじめに
11月29日は何の日@<fn>{nenohi}でしょうか？
そう、いい@<ruby>{NIC, にっく} (Network Interface Card / Network Interface Controller)の日ですね。
近年では2020年東京オリンピックに向けてNICのLEDを制御することが社会に求められており、
ここは平成最後の11月29日、また2018年でたった一日しかない貴重な11月29日に向け、NICのLEDを光らせるべきではないでしょうか。
//footnote[nenohi][子日ちゃん可愛い。]

そこで今回は、@<b>{DPDK} (Data Plane Development Kit)@<fn>{dprk}を用いてNICのLEDを制御します。
DPDKはパケット処理を高速化するライブラリ@<fn>{pdk}で、副次的にユーザランドからNICへ直接アクセスが可能です。
通常は触れられないNICのレジスタ、特にLEDを点灯させるレジスタへもアクセスでき、@<img>{teru}のようなことが実現できます。
本文中ではこうしたLEDの点灯がメインですが、DPDKの概要も紹介を通して皆様の役に立てば幸いです。
//image[teru][照リーヌの様子][scale=0.6]
//footnote[dprk][標準時がUTC+8:30だった国(DPRK)とは関係ありません。たぶん。]
//footnote[pdk][ストレージのパフォーマンスを改善できるSPDK (Storage Performance Development Kit)もあります。Xに好きな文字を入れて君だけのXPDKを作ろう！]

さらに、光とくれば音でしょう。古事記にもそう書かれている。
今回は、コンピュータに備わるブザーで音楽を奏で、それに連動してNICのLEDを光らせることも試しています。
SMF（MIDIファイル）から奏でることができ、主旋律を抽出する方法が分かって楽しかったです@<fn>{konami}。
//footnote[konami][上手に抽出できたとは言っていない。]

それでは、次章から本題に入ります。
まず第2章は、DPDKの概要とNICのLEDを制御するAPIについての紹介です。
第3章は、LEDを制御するための実装に関して、既存の@<tt>{ETHTOOL_PHYS_ID}およびDPDKを利用した場合のそれぞれについて述べています。
第4章が発展課題として音楽演奏やいくつかの課題に取り組んだ結果で、最後の第5章がよく分からないあとがきです。



== DPDKとは
DPDKとは何者なのか、その答えを求め我々は公式サイト@<fn>{dpdk-link}へアクセスした。
曰く、「パケット処理を高速化できるライブラリ」で「Linuxの@<b>{ユーザランド}とFreeBSDで動く」そうです。
Development Kitって書いているし、そんな気はしていました。
//footnote[dpdk-link][@<href>{https://www.dpdk.org/}]

ただ、この説明には少し引っかかるところがあります。
Linuxの場合のパケット処理はカーネルが行うはずで、一部の場合を除きユーザランドで行われません。
一部の場合とは、独自のネットワークスタックを動かしたい、または特殊なルーティングを実現したいときです。
このとき、TAP/TUNデバイスやPacket socket、Raw socket@<fn>{ping}といった機能@<fn>{nado}を利用してユーザランドでパケットを処理しますが、カーネルとの頻繁なメモリコピーといった要因によってパフォーマンスの低下が予期されます。
もしかして、DPDKとは「@<b>{TAP/TUNデバイスやPacket/Raw socketを用いたユーザランドでの}パケット処理を@<b>{CPU拡張命令やビット演算・アクセラレータによって}高速化できるライブラリ」だったのでしょうか？
//footnote[ping][pingもICMP (L3)パケットを送出するために利用しています。]
//footnote[nado][netmap、(e)BPF、XDPは存在しないものとする。]

それはそれで有用だと思いますが、DPDKは「パケット処理を@<b>{カーネルよりも}高速化できるライブラリ」です。
以降では、この高速化を実現するための特徴と、気になるAPIについて述べます。

=== DPDKの特徴
DPDKには、「割り込みの廃止」と「カーネルバイパス」という大きく分けて2つの特徴があります。

==== 割り込みの廃止
LinuxカーネルのNAPI (New API)@<fn>{napi}において、受信したパケットを処理する際のおおまかな流れを次に示します。
//footnote[napi][パケット受信ごとに割り込み処理を行っていた時代に登場した「新しいAPI」です。NAPIの「N」は「Network」ではありません。2005年に実装されており、そろそろ「New NAPI」の登場が待たれます。]

1. NICがパケットを受信し、割り込みを発生させる
2. しばらくポーリングでパケットを処理していく
3. 次の割り込みを待つ

割り込みを待っている間、CPUは他の作業を行えます。
その一方で、割り込みによって作業を切り替える必要があるため、コンテキストスイッチとそれによるオーバヘッドの発生は避けられません。

そこでDPDKではPMD (Poll Mode Driver)@<fn>{mmd}という仕組みによって割り込みを廃止し、すべての処理をポーリングで行っています。
この間CPUは他の作業が行えない、つまりCPU使用率100%となりますが仕様です。
NAPIが登場した頃に比べマルチコアやメニーコア環境が浸透しているとはいえ、「お1コア頂戴つかまつる」というのはなかなか豪快ですね。
//footnote[mmd][MMDことMiku Miku Danceと一緒に語られてそうな名前ですね。]

==== UIOでカーネルバイパス
UIO (Userspace I/O)は、ユーザランドからデバイスを操作できる機能です。
DPDKでは、このUIO@<fn>{vfio}を用いてユーザランドからNICを操作する、つまりカーネルをバイパスします。
こうすることで、ユーザランドとNICのやりとりがそのままDMAで実現できます。
もうNICとカーネルがDMAでやりとりして、さらにカーネルとユーザランドでメモリをコピーする必要はありません。
//footnote[vfio][DPDKでは「UIOよりも安全で高機能」(@<tt>{Linux/Documentation/vfio.txt})なVFIO (Virtual Function I/O)もサポートされています。]

ただ、カーネルをバイパスするということは、NICがカーネルの管理から外れることを意味します。
@<tt>{ifconfig}や@<tt>{ethtool}といったツールからいないものとして扱われ、カーネルのデバイスドライバやプロトコルスタックは利用できません。
とはいえ、デバイスドライバについてはDPDK側で用意されたものがあり、あまり気にしなくて大丈夫です。
プロトコルスタックに関しては、Seastar@<fn>{seastar}というフレームワークを使えば楽をできるかも知れません。
Seastarは、独自のTCP/IPプロトコルスタックを備えており、この上で動くアプリケーションの開発を支援します。
サンプルを見ると@<tt>{httpd}というアプリケーションまであり、恐ろしいフレームワークです。
//footnote[seastar][http://seastar.io/]


=== DPDKのAPI
DPDKは「パケット処理を高速化できる@<bou>{ライブラリ}」であり、さまざまなAPIが存在します@<fn>{dpdk-api}。
たとえば、@<tt>{rte_eth_promiscuous_enable()}ではNICのプロミスキャスモードを有効にできたり、@<tt>{rte_eth_dev_get_eeprom()}でEEPROMの内容を取得可能です。
プロトコルの処理に関しても@<tt>{rte_ipv4_udptcp_cksum()}でチェックサム程度なら計算できるほか、各種プロトコルヘッダの構造体がすでに定義されています。
//footnote[dpdk-api][@<href>{http://doc.dpdk.org/api/}]

さらに、@<tt>{rte_eth_led_on()}や@<tt>{rte_eth_led_off()}といった興味深いAPIもありました。
これらの詳細をドキュメントで確認し、和訳すると次のようになります。
//emlist{
int rte_eth_led_on(uint16_t port_id);
int rte_eth_led_off(uint16_t port_id);

機能
  EthernetデバイスのLEDを点灯/消灯する

引数
  port_id: DPDKが管理するNICのID

返り値
  0: 成功
  -ENOTSUP: デバイスが対応していない
  -ENODEV: port_idが無効
  -EIO: デバイスが削除された
//}
最高ですね。
「EthernetデバイスのLEDを点灯/消灯する」ってことは、つまりすなわち要するに@<img>{rj45}のLEDを点灯/消灯できるということですよ！
NOCさんに対し、LEDを5回点滅させることでア・イ・シ・テ・ルのサインを伝えることも可能です。
HDDのアクセスランプを利用して情報を漏らそうという研究@<fn>{guri2017}では、NICのLEDでも同様のことができるかも知れません。
瞳の奥が熱くなって胸も熱くなる、さすがに気分が高揚します。
ぜひこのAPIを使ってみたいものです。
//image[rj45][RJ45ジャック][scale=0.5]
//footnote[guri2017][@<href>{https://link.springer.com/chapter/10.1007/978-3-319-60876-1_8}]



== 光れ！NICニウム
ということで、DPDKでNICのLEDを光らせましょう。

=== 冴えないNICの照らしかた ～LinuxカーネルSide～
しかし、NICのLEDを光らせるというのは本当にDPDKを使わないと実現できないのでしょうか？
そこで、まずはDPDKを使わずにNICの制御をLinuxカーネルに任せた場合でもLEDを光らせられるのか検討します。
ただ、先に結論を述べると、LEDを自由に制御するためにはカーネルの改変が必要で敷居は高いことが分かりました。

通常、NICのLEDが光って嬉しいときというのは、@<tt>{eth0}や@<tt>{enp1s0}といったデバイス名に紐づくNICの物理的な位置を知りたい場合でしょう。
これを実現するのが「@<tt>{ethtool -p <デバイス名>}」というコマンドで、NICのLEDを一定間隔で点滅させます。
この機能は@<tt>{ioctl}システムコールを呼び@<fn>{ioctl}実現しているということが、@<tt>{strace}や@<list>{ethtool}に示す@<tt>{ethtool}のソースコードから分かりました。
//list[ethtool][ethtoolから抜粋したソースコード]{
static int do_phys_id(int fd, struct ifreq *ifr)
{
    int err;
    struct ethtool_value edata;

    edata.cmd = ETHTOOL_PHYS_ID;
    edata.data = phys_id_time;
    ifr->ifr_data = (caddr_t)&edata;
    err = ioctl(fd, SIOCETHTOOL, ifr);
    if (err < 0)
        perror("Cannot identify NIC");

    return err;
}
//}
//footnote[ioctl][厳密には「@<tt>{ioctl}システムコールをラップしたglibcの@<tt>{ioctl()}関数」を呼んでいるのですが見逃してください。]

この@<tt>{ioctl()}を実行すると、カーネルの@<tt>{net/core/ethtool.c}に定義される@<tt>{ethtool_phys_id()}関数が呼ばれます。
この関数のソースコードは@<list>{ethtool_phys_id}に示すとおりで、このコードからデバイスドライバの@<tt>{set_phys_id(dev, ETHTOOL_ID_ACTIVE)}の返り値から決定した周期にしたがい、@<tt>{set_phys_id(dev, ETHTOOL_ID_OFF)}または@<tt>{set_phys_id(dev, ETHTOOL_ID_ON)}によってLEDを点滅させていました@<fn>{ldd}。
//listw[ethtool_phys_id][ethtool_phys_id()のソースコード（抜粋）]{
static int ethtool_phys_id(struct net_device *dev, void __user *useraddr)
{
    int rc = ops->set_phys_id(dev, ETHTOOL_ID_ACTIVE);
    int n = rc * 2, i, interval = HZ / n;

    do {
        i = n;
        do {
            rtnl_lock();
            rc = ops->set_phys_id(dev, (i & 1) ? ETHTOOL_ID_OFF : ETHTOOL_ID_ON);
            rtnl_unlock();
            if (rc)
                break;
            schedule_timeout_interruptible(interval);
        } while (!signal_pending(current) && --i != 0);
    } while (!signal_pending(current) && (id.data == 0 || --id.data != 0));

    (void) ops->set_phys_id(dev, ETHTOOL_ID_INACTIVE);
    return rc;
}
//}
//footnote[ldd][デバイスドライバ側で点滅させるNICもあるようですが、忘れたことにします。]

@<tt>{set_phys_id()}は関数ポインタであり、たとえばe1000ドライバの場合@<tt>{drivers/net/ethernet/intel/e1000/e1000_ethtool.c}に定義される@<tt>{e1000_set_phys_id()}が呼び出されます。
ソースコードは@<list>{e1000_set_phys_id}に示すとおりで、@<tt>{ETHTOOL_ID_ACTIVE}を指定して呼び出されたときは定数@<tt>{2}を返しており250 [ms]間隔でLEDの点滅が変化すること、また@<tt>{ETHTOOL_ID_ON}や@<tt>{ETHTOOL_ID_OFF}が指定されると@<tt>{e1000_led_on()}関数や@<tt>{e1000_led_off()}関数を呼び出していることが判明しました。
なお、@<tt>{e1000_led_on()}関数や@<tt>{e1000_led_off()}関数の中では、特定のレジスタを操作しています。
これによって、RJ45ジャックに内蔵されたLEDに向けて電圧が印加され、点灯状態が変化するわけです。
//list[e1000_set_phys_id][e1000_set_phys_id()のソースコード（抜粋）]{
static int e1000_set_phys_id(struct net_device *netdev,
                 enum ethtool_phys_id_state state)
{
    switch (state) {
    case ETHTOOL_ID_ACTIVE:
        e1000_setup_led(hw);
        return 2;
    case ETHTOOL_ID_ON:
        e1000_led_on(hw);
        break;
    case ETHTOOL_ID_OFF:
        e1000_led_off(hw);
        break;
    case ETHTOOL_ID_INACTIVE:
        e1000_cleanup_led(hw);
    }
    return 0;
}
//}

以上をまとめると、@<tt>{ethtool -p}による操作でのLEDは点滅以外できず、しかもその周期はデバイスドライバによって決められているということになります。
そして、実は@<tt>{e1000_led_on()}関数や@<tt>{e1000_led_off()}関数をたとえ間接的にでもユーザランドから呼ぶには@<tt>{ethtool -p}による操作以外なく、ユーザランドからLEDを自由に制御することはできません。
どうしても自由なLED制御をユーザランドから実現したい場合、@<tt>{net/core/ethtool.c}の改変が必要となり、気軽に試すわけにもいかなくなります。


=== 照-Teru- DPDK編
やはりDPDKから照らすのが最高なんですね。
ということで、次からはその動作環境について説明し、続いて環境構築とDPDKアプリケーションの作成について述べます。

==== 動作環境
まず、DPDKが動作する環境において最も重要なハードウェアはNICです。
DPDKは以前@<b>{Intel DPDK}という名前だったこともあり、2012年に初めて公開されたバージョン1.2.3r0@<fn>{dpdk-v1.2.3r0}ではigb (e1000)とixgbeにしか対応していませんでした。
現在ではIntel以外の物理NICのほか、QEMUのvirtio-netやEC2のENA (Elastic Network Adapter)といった仮想NICにも対応しています。
どうせなら物理NICで動かしたいところですが、Intel以外のNICは10GbEやFPGA付きばかりなので覚悟が必要です。
特に目的がなければ@<fn>{tukkomi}、Amazonで新品か中古かそもそも本物なのか分からない謎のIntel NICを買う方針でも良いと思います。
このとき複数のポートがあると、スイッチやルータのサンプルアプリケーションが動かしやすくなるかも知れませんね。
そうして筆者は「Intel PRO/1000 PT Dual Port Server Adapter EXPI9402PT (Intel 82571EB Gigabit Ethernet Controller)」を4,930円で購入しました。
//footnote[dpdk-v1.2.3r0][@<href>{http://git.dpdk.org/dpdk/log/?h=v1.2.3r0}]
//footnote[tukkomi][目的がない人がDPDKをやるかどうかはさておき。]

次にCPUです。
DPDKではx86 (x86_64)やARMのほか、POWERまでサポートされています。
特にx86に注目すると、次の条件を満たす必要がありますが最近のCPUであれば問題ありません。

: マルチコアCPU
  DPDKに1コアを持っていかれるためです。
  もっとも、今となってはわざわざシングルコアCPUを用意するほうが難しいと思います。
: Hugepages対応
  DPDKではHugepagesを利用しています@<fn>{no-huge}。
  Hugepagesとは、仮想記憶におけるページサイズを通常の4KBから2MBや1GBに拡張できる仕組みのことです。
  これによって任意アドレスの変換情報がTLBに収まるほど少数のページテーブルに集約されやすくなり、TLBミスの抑制が期待できます。
  この仕組みはCPU側の対応が必要で、@<tt>{/proc/cpuinfo}の@<tt>{flags}に@<tt>{pse} (2MB)または@<tt>{pdpe1gb} (1GB)があれば大丈夫です。
  なお、DPDKのドキュメントによると「Hugepagesは全体で2GBほど予約すべし」といった記述があります。
  しかし、もっと小さくしても動くので、メモリが足りなくても心配する必要はありません。
: SSE4.2対応
  DPDK v17.08以降では、命令セットとしてSSE4.2を備えたCPUを要求するようになりました。
  とはいえ、Intel Core iシリーズの第一世代(Nehalem)から実装されており、よほど古いCPUでなければ気にする必要はないでしょう。
//footnote[no-huge][@<tt>{--no-huge}オプションもあるようですが、見なかったことにします。]

ところが今回用意できたCPUは「Intel Celeron E3400」という@<b>{よほど古いCPU}であり、SSE4.2対応の条件を満たせませんでした。
ただ、DPDK v17.05では、SSE4.2ではなくSSE3を備えたCPUであれば利用可能です。
そこで、そのバージョンを利用し、用意したCPUを使うことにします。
DPDKが管理するポートIDが@<tt>{uint8_t}から@<tt>{uint16_t}に拡張されたなど多少の違いはありますが、やむを得ません。

以上を踏まえつつ動作環境を決めると、@<table>{table-environment}に示すとおりになりました。
//tabooular[table-environment][動作環境]{
=========>|<=======================================
CPU       | Intel Celeron E3400 (Dual Core, pse, SSE3)
Memory    | 2GB
NIC       | Intel 82571EB (Dual 1GbE)
----------------
OS        | Ubuntu 18.04 LTS Server
DPDK      | v17.05
Hugepages | 2MB@<m>{\times}512Pages
//}

==== 環境構築
ソフトウェアのセットアップについて述べます。

まずやることはDPDKのビルドです。
次に示すようにいくつかアプリケーションをインストールして、ソースコードからビルドします。
//cmd{
$ sudo apt install build-essential libcap-dev python
$ wget http://fast.dpdk.org/rel/dpdk-17.05.2.tar.gz
$ tar zxvf dpdk-17.05.2.tar.gz
$ cd dpdk-stable-17.05.2/ && \
        make install T=x86_64-native-linuxapp-gcc
//}

その次はHugepagesの設定です。
次に示すコマンドを実行します。
//cmdw{
$ sudo sed -ie 's/\(GRUB_CMDLINE_LINUX=\)/#\1/g' /etc/default/grub
$ echo 'GRUB_CMDLINE_LINUX="hugepages=512"' | sudo tee -a /etc/default/grub
$ sudo grub-mkconfig -o /boot/grub/grub.cfg
$ sudo mkdir -p /mnt/huge
$ echo 'nodev /mnt/huge hugetlbfs defaults 0 0' | sudo tee -a /etc/fstab
$ sudo reboot
//}

さらにNICの設定です。
次に示すように、NICをカーネルからDPDKの管理下におきます。
この操作は再起動すると元に戻るので気をつけましょう。
なお、@<tt>{modprobe}については@<tt>{/etc/modules}にモジュール名を書いておけば起動時にロードしてくれます。
@<tt>{dpdk-devbind.py}についてはsystemdのサービスを書いて起動時に実行するという方法があるようです。
//cmdw{
$ sudo modprobe uio_pci_generic
$ sudo ~/dpdk-stable-17.05.2/usertools/dpdk-devbind.py --status
$ sudo ~/dpdk-stable-17.05.2/usertools/dpdk-devbind.py --bind=uio_pci_generic 0000:01:00.0
$ sudo ~/dpdk-stable-17.05.2/usertools/dpdk-devbind.py --bind=uio_pci_generic 0000:01:00.1
$ sudo ~/dpdk-stable-17.05.2/usertools/dpdk-devbind.py --status
//}

そして最後に環境変数の設定を行います。
内容は次のとおりです。
@<tt>{~/.bash_profile}かどこかに記述しても構いません。
//cmd{
$ export RTE_SDK=~/dpdk-stable-17.05.2/
$ export RTE_TARGET=x86_64-native-linuxapp-gcc
//}

これまでの操作でDPDKアプリケーションのビルドと実行が可能になります。
実際、次のようなコマンドが実行できるはずです。
//cmd{
$ cd $RTE_SDK/examples/helloworld
$ make
$ sudo ./build/helloworld
//}

==== DPDKアプリケーションの作成
やや大げさなタイトルですが、NICのLEDを光らせるためのDPDKアプリケーションを作成します。
前もって作成しておいたものがこちらです。
//emlist{
lrks/hikare-nicnium
https://github.com/lrks/hikare-nicnium
//}

ここでは、DPDKの@<tt>{examples/ethtool}に含まれる初期化処理を流用しました。
主な処理は@<list>{blink}のとおりとなり、非常に簡単になります。
@<tt>{rte_eth_led_on()}と@<tt>{rte_eth_led_off()}を叩いているだけなので当然ですね。
//list[blink][DPDKでLEDを点滅させるコード]{
static void control_led(uint8_t port_id, int flg)
{
    flg ? rte_eth_led_on(port_id) : rte_eth_led_off(port_id);
}

void nicapp_main(uint8_t cnt_ports)
{
    int i;
    uint8_t id;

    for (i=0; i<10; i++) {
        for (id=0; id<cnt_ports; id++)
            control_led(id, (id + i) % 2);
        sleep(1);
    }

    for (id=0; id<cnt_ports; id++)
        control_led(id, 0);
}
//}

@<list>{blink}を実行すると、@<img>{blink1}や@<img>{blink2}のようにNICのLEDが交互に点灯します。
//subfig[LEDが点灯する様子]{
//image[blink1][あるNICのLEDが光る][scale=0.4925]
//image[blink2][異なるNICのLEDが光る][scale=0.49]
//}



== 発展課題
ただ単にLEDを光らせただけでは物足りません。
そこで、3つの発展課題をこなしていきます。
ただし、先に述べておくと最初のふたつは「要再提出」、最後がかろうじて@<emoji>{accept}という感じです。

=== PWM制御
LチカといえばPWMでしょう。
LEDをPWMで制御すれば、点灯または消灯という状態に加えて「ほのかに光る」「そこそこ明るい」といった中間の状態を擬似的に作り出せます。
これにはある期間における「LEDが点灯している時間」と「消灯している時間」の比率を制御する必要があり、今回のDPDKでは次のようなコードで実現できる@<fn>{sched}と考えていました。
//emlist{
static void led_pwm(uint8_t port_id, int ratio)
{
    int i;
    int on = ratio % (10 + 1);
    int off = 10 - on;

    while (1) {
        for (i=0; i<on; i++) rte_eth_led_on(port_id);
        for (i=0; i<off; i++) rte_eth_led_off(port_id);
    }
}
//}
//footnote[sched][RTOSではないため精度は悪くなりますが、承知の上です。]

ところが、このコードを実行すると常にLEDが点灯@<fn>{osc}してしまいます。
原因は不明ですが、NIC側のレジスタ@<fn>{nic-reg}へのアクセスが頻繁に行えないような、または消灯よりも点灯が優先されるような印象を受けました@<fn>{tabun}。
一定時間待てばきちんと点灯と消灯が可能でしたが、この一定時間というのは200msなど点滅が目視できるほど長く、PWM制御は諦めざるを得ません。
ドライバを改変@<fn>{dpdk-driver}し、レジスタの書き込みに使われる@<tt>{E1000_WRITE_REG()}マクロの代わりに@<tt>{E1000_PCI_REG_WRITE_RELAXED()}+@<tt>{E1000_PCI_REG_ADDR()}マクロや@<tt>{E1000_WRITE_FLUSH()}というそれらしい名前のマクロも使ってみたものの、効果はありませんでした。残念。
//footnote[osc][わざわざオシロスコープで観測したので自信があります。]
//footnote[nic-reg][主語が大きいので「少なくともLED状態を設定するレジスタ」としておきます。]
//footnote[tabun][データシートを読み込んでおらず、本当か疑わしいので「擬似的にそう見える」としてください。ちょうどPWMの話なので。]
//footnote[dpdk-driver][DPDKなら改変したドライバの適用も簡単！]



=== 光れ！NICニウム～ethtoolこうこうこうこう部へようこそ～
少し前に「ethtoolではNICのLEDを自由に制御できない」と述べました。
しかし、本当にそうでしょうか？
すでに@<list>{ethtool_phys_id}で示した@<tt>{ethtool_phys_id()}について、その抜粋を@<list>{ethtool_phys_id_2}に示します。
//listw[ethtool_phys_id_2][ethtool_phys_id()のソースコード（抜粋）]{
int rc = ops->set_phys_id(dev, ETHTOOL_ID_ACTIVE);

int n = rc * 2, i, interval = HZ / n;
do {
    i = n;
    do {
        rc = ops->set_phys_id(dev, (i & 1) ? ETHTOOL_ID_OFF : ETHTOOL_ID_ON);
        schedule_timeout_interruptible(interval);
    } while (!signal_pending(current) && --i != 0);
} while (!signal_pending(current));

ops->set_phys_id(dev, ETHTOOL_ID_INACTIVE);
//}

おおまかな処理の流れは次のとおりです。

1. @<tt>{set_phys_id(dev, ETHTOOL_ID_ACTIVE)}でLEDの点灯状態を保存
2. @<tt>{i}に偶数をセットする
3. @<tt>{set_phys_id(dev, ETHTOOL_ID_ON)}でLEDを点灯
4. @<tt>{schedule_timeout_interruptible(interval)}で@<tt>{interval} [tick]間sleepしつつ、処理すべきシグナルが来たら起きる
5. @<tt>{signal_pending(current)}でシグナルが来ているか調べる
6. 来ていたら処理を抜けて10.へ、何もなければ@<tt>{i}に奇数をセット
7. @<tt>{set_phys_id(dev, ETHTOOL_ID_OFF)}でLEDを消灯
8. 4.から6.までとほぼ同じ処理を行う
9. 2.に戻る
10. @<tt>{set_phys_id(dev, ETHTOOL_ID_INACTIVE)}で1.の状態を復元

よく見ると、4.のときにSignalを配送すればLEDの点灯時間を0から@<tt>{interval}tickまで任意に設定できます。
その後にすぐ@<tt>{ethtool_phys_id()}を呼び直せば、より長い時間LEDが点灯しているように見えるかもしれません。
また、LEDの消灯時間は1.でLEDが消灯している、すなわちNICがLinkUpしていないならばもちろん任意で、これは10.によって直前の点灯状態に依存しないはずです。

これらを踏まえ、まずは点灯のみに注目した実験コード（@<list>{hikare-ethtool}）を書いてみました。
ところが、このコードを実行しても期待通りの動作はしません。
まず、常時点灯を試すと頻繁に@<tt>{EPERM} (Operation not permitted)が返され、反対に点灯時間を短くしようとすると無視されて引き伸ばされます。
もっとも、後者に関しては前項の「PWM制御」で述べたのと同じ挙動という印象で、ethtoolとDPDKのどちらを使っても回避できないのかも知れません。
残念！要再提出です。でもたぶん発展課題なので履修放棄します。
//list[hikare-ethtool][ethtool_phys_id()でLEDを光らせようとしたコード（抜粋）]{
#define ETHTOOL_LED_VALUE(v) ((struct ethtool_led_value *)(v))
struct ethtool_led_value {
    int fd;
    struct ifreq *ifr;
    int status;
};

void *ethtool_led_on(void *args)
{
    int oldtype;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);

    struct ethtool_value edata;
    edata.cmd = ETHTOOL_PHYS_ID;
    edata.data = 0;
    ETHTOOL_LED_VALUE(args)->ifr->ifr_data = (caddr_t)&edata;

    int err = ioctl(ETHTOOL_LED_VALUE(args)->fd,
                    SIOCETHTOOL, ETHTOOL_LED_VALUE(args)->ifr);
    ETHTOOL_LED_VALUE(args)->status = err;
    return args;
}

int main(int argc, char *argv[])
{
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, argv[1], IFNAMSIZ);

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct timespec req = { 0, 100 * (1000 * 1000) }; // 100ms
    struct ethtool_led_value args = { fd, &ifr, 0 };
    while (args.status >= 0) {
        pthread_t th;
        pthread_create(&th, NULL, ethtool_led_on, (void *)&args);
        nanosleep(&req, NULL);
        pthread_cancel(th);

        // sleepなし:常時点灯, あり:単発で100msだけ光らせる
        sleep(1);
    }

    close(fd);
    return 0;
}
//}



=== BUZとioctl
音楽を奏でつつ、NICのLEDが光ったら面白いと思いました。

コンピュータで音を鳴らすといえば、pcspkr@<fn>{pcspkr}を@<tt>{ioctl()}で操作するのが一般的です[要出典]@<fn>{beep}。
@<tt>{modprobe pcspkr}して@<list>{pcspkr}のようなコードを実行すると、@<tt>{freq} [Hz]の音が1秒間鳴ります。
和音は出せない、つまり同時発音数は1つだけ@<fn>{psg}ですが意外と綺麗な音です。
なお、たまにコンピュータに備わるブザーではなく、ALSAから音が出る場合があります。
これは、サウンドカードを検出するとブザー音がエミュレートされてしまうためで、回避するにはカーネルのリビルドが必要@<fn>{alsa}のようです。
//list[pcspkr][pcspkrを操作するコード（抜粋）]{
#define DEVICE_CONSOLE "/dev/tty0"
#define CLOCK_TICK_RATE 1193180

void pcspkr(int freq)
{
    int fd = open(DEVICE_CONSOLE, O_WRONLY);
    ioctl(fd, KIOCSOUND, (CLOCK_TICK_RATE / (double)freq));
    sleep(1);
    ioctl(fd, KIOCSOUND, 0);
    close(fd);
}
//}
//footnote[pcspkr][BeepやBZ (Buzzer)のことです。]
//footnote[beep][わざわざこんなことしなくとも@<tt>{beep}コマンドで鳴らせます。]
//footnote[psg][時分割で擬似和音を奏でていた時代もあったようですが…。]
//footnote[alsa][@<href>{https://pasero.net/~mako/blog/s/110}]

ここに周波数と時間を書いていけば音楽を奏でられますが、手でひとつずつ書いていくのは非常に手間です。
そこで、SMF (Standard MIDI File)を基に奏でて楽をしましょう。
SMFとは、後述する「MIDIイベント」とそれを発行するタイミングが記録された、いわゆるMIDIファイルです。
イベントとタイミングはトラック(Track)という場所に格納されます。
SMF (Format 0)を除くSMF (Format 1)またはSMF (Format 2)では最大256本のトラックを保持でき、各トラックに格納されたデータは他トラックと独立です。
ここに含まれるMIDIイベントのうち、主なものは次のとおりとなっています。

: ノートオン (@<tt>{0x9n kk vv})
  ノートナンバー@<tt>{kk}(key, 0~127)の音をチャンネル@<tt>{n} (0~15)@<fn>{channel}で鳴らす。
  ピアノの鍵盤を押し込む速度（音の大きさ）を@<tt>{vv} (velocity, 0~127)で指定する。
  @<tt>{vv}を@<tt>{00}とすると、次に述べる「ノートオフ」とほぼ同じとなる。
: ノートオフ (@<tt>{0x8n kk vv})
  チャンネル@<tt>{n}で鳴っているノートナンバー@<tt>{kk}の音を止める。
  鍵盤から手を離す速度@<tt>{vv}を指定する。
: ポリフォニックキープレッシャー (@<tt>{0xAn kk vv})
  チャンネル@<tt>{n}で鳴っているノートナンバー@<tt>{kk}の音を速度@<tt>{vv}で発音し直す。
  ノートオンと同様に、@<tt>{vv}が@<tt>{00}ならばノートオフと同じになる。
: チャンネルプレッシャー (@<tt>{0xDn vv})
  チャンネル@<tt>{n}で鳴っているすべての音を速度@<tt>{vv}で発音し直す。
  @<tt>{vv}が@<tt>{00}ならばチャンネル@<tt>{n}の音をすべて消音する。
: プログラムチェンジ (@<tt>{0xCn pp})
  チャンネル@<tt>{n}で鳴らす音のプログラム（音色）を@<tt>{pp} (program, 0~127)に変える。
  音色にはピアノや打楽器などがあり、番号との対応は機器がサポートする規格によって決まる。
: ピッチベンド (@<tt>{0xEn mm ll})
  チャンネル@<tt>{n}のピッチ（音高）を微調整する。
  @<tt>{mm}と@<tt>{ll}はともに7bit、@<tt>{mm}をMSB、@<tt>{ll}をLSBとして0~16383@<fn>{pb}までのデータを構成する。
: オールサウンドオフ / オールノートオフ (@<tt>{0xBn 78} / @<tt>{0xBn 7B})
  チャンネル@<tt>{n}で鳴っているすべての音を止める。
: リセットオールコントローラ (@<tt>{0xBn 79})
  チャンネル@<tt>{n}について設定値を初期化する。
//footnote[channel][機器アドレスのようなものです。GM (General MIDI)という規格では10番目のチャンネルが打楽器専用など、チャンネル番号自体が意味を持つこともあります。]
//footnote[pb][ここでは初期値を8192とします。範囲を-8192~8191として初期値0とすることも多いようです。]

なお、pcspkrに渡す周波数はノートナンバーとピッチを基に次のように計算できます@<fn>{freq}。
//texequation{
440 \times 2^{\left(\frac{note-69}{12}+\frac{pitch-8192}{4096\times12}\right)}
//}
//footnote[freq][@<href>{https://dsp.stackexchange.com/questions/1645/converting-a-pitch-bend-midi-value-to-a-normal-pitch-value}]

これを踏まえてSMFをパースしていきます。
Cは辛いので、一度PythonからパースしてCで扱いやすいテキスト形式に変換しましょう。
mido@<fn>{mido}というライブラリを用いて実装@<fn>{doro}したものがこちらになります。
//emlistw{
https://github.com/lrks/hikare-nicnium/blob/master/pcspkr/mid2txt.py
$ ./mid2txt.py --help
usage: mid2txt.py [-h] [-m METHOD] inputFile [outputFile]
//}
//footnote[mido][@<href>{https://github.com/olemb/mido}]
//footnote[doro][泥臭い処理を書くことになります。こういうときだけ心の平穏が得られる。]

そういえば、以前にpcspkrでは和音が出せないことを述べました。
これでは、Monophonic（単音） MIDIしか奏でられません。
そこで、Polyphonic（多音） MIDIも@<bou>{それなりに}奏でられるよう「先着順」で音を鳴らしています。
その様子が@<img>{simple}で、発音中に来たイベントは無視されて流れていきます。
流しそうめんみたいですね。
//image[simple][MIDIイベントを「先着順」で発音する様子]

しかし、この方法では主旋律ではなく伴奏だけが鳴ってしまい、曲がよく分からなくなってしまいそうです。
実は、Polyphonic MIDIから主旋律を抽出してMonophonic MIDIにする@<fn>{mir}のは音楽情報検索(MIR)において必要なタスクのようで、そのためのアルゴリズムがいくつか提案されています。
その中にあるのがSkyline@<fn>{skyline}と呼ばれる手法です。
この手法では@<img>{skyline}のように「先着順」かつ「なるべく高い音」を鳴らそうとします@<fn>{asyu}。
実際に試してみると、主旋律の音高が高い曲@<fn>{animesong}においては、ほぼ主旋律だけを鳴らせました。
どのような感じか記録した動画があり、次のような感じです。
//image[skyline][Skylineによる発音の様子]
//footnote[mir][正確には「特徴的な情報を抽出し検索しやすい形にする」処理です。]
//footnote[skyline][@<href>{https://dl.acm.org/citation.cfm?id=319470}で提案された「All-Mono」が基になっているようです。]
//footnote[asyu][一回の発音時間を制限するなどいくつかの亜種があるようです。君だけのSkylineを作ろう！]
//footnote[animesong][アニソンとボカロ曲です。]
//emlist{
Computer Music with Link LED
https://www.youtube.com/watch?v=tmEozHUE-tI
//}

ただ、やはりそれ以外の曲も奏でたかったため他の手法も試してみます。
具体的には、特徴的なチャンネルを選択・結合してSkylineを適用する手法@<fn>{ozcan2005}や、ノートの数や種類から主旋律のトラックやチャンネルを抽出する手法@<fn>{velusamy2007}です。
しかし、あまり良い結果は得られませんでした。
期待していたよりも伴奏が抽出されてしまい、奏でる曲がよく分からなくなってしまいます。
とはいえ、前処理として述べられていた「打楽器は伴奏なので除く」「発音時間が0.05秒など短すぎるノートはノイズの可能性が高い」というコツは有効でした。
これをSkylineと合わせるといい感じになります。お役立ちです。
//footnote[ozcan2005][@<href>{https://ieeexplore.ieee.org/abstract/document/1565863/}]
//footnote[velusamy2007][@<href>{https://link.springer.com/chapter/10.1007/978-3-540-69429-8_25}]

でも、欲をいえばAIってやつでなんとかしてほしいですね@<fn>{sigcoww}。
もっといえばWAVファイルからMonophonic MIDIや、同時発音数を@<m>{n}に制限したMIDIファイルを作成してほしいです。
AIでなんとかできませんか@<fn>{sigcoww2}。
//footnote[sigcoww][こう書いておくと他のSIGCOWWメンバーがなんとかしてくれるんじゃないですかね。]
//footnote[sigcoww2][他のメンバーが以下略。2回書いたのでアピールが2倍になった。]



== おわりに
NICのLEDを光らせることができて良かったです（小並感）。

ところで、SRG46ってご存知ですか？
@<b>{空想上の}IPv4/v6トランスレータで、ペイロードまで変換するALG (Application Layer Gateway)の一種です。
いま、あなたはDPDKを知っているはずであり、もしかしたらSRG46が現実のものとなるかも知れませんよ。
本文中で存在しないものとしたnetmapほか流行りのP4@<fn>{p4}も使って君だけのSRG46を作ろう！
でも、元ネタ@<fn>{nareruse}ではIPv4/v6アドレスのマッピングを自動で行っていましたね。
これの実現方法？こまけぇこたぁいいんだよ！！
//footnote[p4][@<href>{https://p4.org/} （ペルソナ4ではない）]
//footnote[nareruse][夏海公司, Ixy. "なれる！SE 9." KADOKAWA アスキー・メディアワークス. 2013.]
