#!/bin/sh
# ex1) $ ./make.sh build --help
# ex2) $ env CONTAINER_VERSION="debug" ./make.sh build --help
# PDF/EPUBを作成する
#  * Dockerコンテナの実行環境が必要
#  * 結果は make.sh と同じ階層の working_temporary_directory/ に書く
#  * src/working_temporary_directory/ で作業するので強制終了したら残るかも
# 環境変数 $CONTAINER_VERSION を読む
#  * ビルド用コンテナのタグを指定
#  * 無指定なら circle.yml を基に決定
#
# ex1) $ ./make.sh install HOGE
# HOGE章を追加する
#  * src/articles/HOGE/HOGE.re を作成        (HOGE.re が存在しない場合)
#  * src/catalog.yml の CHAPS へ HOGE を追加 (HOGE が存在しない場合)
#  * src/articles/HOGE/images/ を作成        (src/images/HOGE が存在しない場合)
#
# ex1) $ ./make.sh remote
# リモートで ./make.sh build を実行する
#  * 本当に ./make.sh build のみ
#  * オプションも通らない


#
# build
#
build() {
	set +u
	ENV_VER=$CONTAINER_VERSION
	CFG_VER=$(grep "^\\s*container_version:" src/config.yml | head -1 | sed -r "s/.*?[\"'](.*?)[\"'].*/\\1/")
	CI_VER=""
	if [ -f ".circleci/config.yml" ]; then
		CI_VER=$(grep "image:" .circleci/config.yml | head -1 | sed -r 's/.*?lrks\/desk:(.+?)\s*/\1/')
	fi

	# array is undefined in POSIX
	if [ -n "$ENV_VER" ]; then
		CONTAINER_VERSION=$ENV_VER
	elif [ -n "$CFG_VER" ]; then
		CONTAINER_VERSION=$CFG_VER
	elif [ -n "$CI_VER" ]; then
		CONTAINER_VERSION=$CI_VER
	else
		echo "\\033[33m\\033[41mCONTAINER_VERSION: value is blank\\033[m\\033[m"
	fi

	# ARRAY IS UNDEFINED IN POSIX
	if [ -n "$ENV_VER" ] && [ "$ENV_VER" != "$CONTAINER_VERSION" ]; then
		echo "\\033[33m\\033[41mCONTAINER_VERSION: expected \"${ENV_VER}\" (at \$ENVVAR) but was \"${CONTAINER_VERSION}\"\\033[m\\033[m"
	elif [ -n "$CFG_VER" ] && [ "$CFG_VER" != "$CONTAINER_VERSION" ]; then
		echo "\\033[33m\\033[41mCONTAINER_VERSION: expected \"${CFG_VER}\" (at src/config.yml) but was \"${CONTAINER_VERSION}\"\\033[m\\033[m"
	elif [ -n "$CI_VER" ] && [ "$CI_VER" != "$CONTAINER_VERSION" ]; then
		echo "\\033[33m\\033[41mCONTAINER_VERSION: expected \"${CI_VER}\" (at .circleci/config.yml) but was \"${CONTAINER_VERSION}\"\\033[m\\033[m"
	fi

	if [ ${#CONTAINER_VERSION} -eq 64 ]; then
		container="lrks/desk@sha256:${CONTAINER_VERSION}"
	elif [ ! -n "$CONTAINER_VERSION" ]; then
		container="lrks/desk"
	else
		container="lrks/desk:${CONTAINER_VERSION}"
	fi

	if [ -n "$PAPER_MARGIN" ]; then
		PAPER_MARGIN="--margin=${PAPER_MARGIN}"
	else
		if [ -f ".circleci/config.yml" ]; then
			PAPER_MARGIN=$(grep "margin" .circleci/config.yml | head -1 | sed -r 's/.*?(--margin=[0-9]+mm).*/\1/');
		else
			PAPER_MARGIN=""
		fi
	fi
	set -u


	cmd="build.rb ${*} --workdir=/work ${PAPER_MARGIN}"
	echo "\\033[35mcmd: [ ${cmd} ]\\033[m"
	set +e
	ret=$(id -Gn | grep "docker")
	set -e
	if [ "$ret" ]; then
		docker run --rm -v "$(pwd)/src/:/work" "$container" /bin/ash -c "$cmd"
	else
		sudo docker run --rm -v "$(pwd)/src/:/work" "$container" /bin/ash -c "$cmd"
	fi
	exitstatus=$?

	if [ -d "src/working_temporary_directory" ]; then
		exitstatus=$(sed -n 1P src/working_temporary_directory/.exitstatus)
	fi
	exit "$exitstatus"
}

#
# install
#
install() {
	cd src/ || return

	# .re
	if [ ! -e "articles/${1}/${1}.re" ]; then
		mkdir -p "articles/${1}"
		cat << EOF > "articles/${1}/${1}.re"
= ${1}
//lead{
前文
//}

== ほげほげ
EOF
	fi

	# catalog.yml
	if [ "$(grep -c "${1}.re" catalog.yml)" -eq 0 ]; then
		chap_num=$(grep 'CHAPS:' catalog.yml -n | cut -d':' -f1)
		set +u
		insert_start=$(grep ':' catalog.yml -n | cut -d':' -f1 | while read -r num; do
			if [ "$num" -gt "$chap_num" ]; then
				echo "$num"
				break
			fi
		done)

		if [ ! -n "$insert_start" ]; then
			echo "  - ${1}.re" >> catalog.yml
		else
			sed -i "${insert_start}i\\ \\ - ${1}.re" catalog.yml
		fi
		set -u
	fi

	# src/images
	mkdir -p "articles/${1}/images/"
}

#
# remote
#
remote() {
	DIFF=".temporary.diff"

	repo=$(basename "$(git rev-parse --show-toplevel)")
	origin=$(git log -1 origin/master --pretty=format:"%H")
	headline="#${repo}:${origin}"
	echo "$headline" > $DIFF
	set +e
	git diff --binary "$origin" >> $DIFF
	git ls-files --others --exclude-standard | xargs -L1 -I% git diff --no-index --binary /dev/null % >> $DIFF
	set -e

	< $DIFF ssh sigcoww@docker.sigcoww.org -p 25252 -C | awk "{if(\$0~/^+>#/){print substr(\$0,4,length(\$0))>\"${DIFF}\"}else{print}}"
	if [ "$(grep -c "$headline" "$DIFF")" -eq 0 ]; then
		rm -rf "src/working_temporary_directory"
		git apply $DIFF
	fi

	rm $DIFF
}


#
# Caller
#
if [ $# -eq 0 ]; then
	echo "ex-1) ${0} build --help"
	echo "ex-2) ${0} install hoge"
	echo "ex-3) ${0} remote"
	exit 1
fi

set -eu
cd "$(dirname "$0")" || return
cmd=$1
shift
case "$cmd" in
"build") build "$@" ;;
"install") if [ $# -eq 1 ]; then install "$1"; fi ;;
"remote") remote ;;
*) echo "HA?"
esac
