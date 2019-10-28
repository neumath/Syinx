
_var1_event=0
_var2_pthread=0
_var3_mysqlclient=0

if [ $(ldconfig -p | grep -c libevent ) -gt 0 ]
then
    echo "find libevent"
    _var1_event=1
else
    echo "libevent is not find"
fi


if [ $(ldconfig -p | grep -c libpthread ) -gt 0 ]
then
    echo "find libpthread"
    _var2_pthread=1
else
    echo "libpthread is not find"
fi

if [ $(ldconfig -p | grep -c libmysqlclient ) -gt 0 ]
then
    echo "find libmysqlclient"
    _var3_mysqlclient=1
else
    echo "libmysqlclient is not find"
fi


if [ $(ldconfig -p | grep -c libprotobuf ) -gt 0 ]
then
    echo "find libprotobuf"
    _var4_protobuf=1
else
    echo "libprotobuf is not find"
fi

_var_ok=3

if test $[_var1_event+_var2_pthread+_var3_mysqlclient] -eq $[_var_ok]
then
    ./SyinxMake ./main.cpp
    mv main.cpp ../main.cpp
    echo " \0/ ok!"
    echo "Please run cmake ."
else
    echo " QAQ failed!"
fi












