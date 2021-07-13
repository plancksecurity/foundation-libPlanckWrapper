#!/bin/bash
set -exuo pipefail

# ===========================
# Distro
# ===========================

echo 7 >"${INSTPREFIX}/D_REVISION"

D_REV=$(cat ${INSTPREFIX}/D_REVISION)
D=""

D=${INSTPREFIX}/out

mkdir -p ${INSTPREFIX}/out
rm -rf ${INSTPREFIX}/out/*
mkdir -p "$D"/{bin,ld,lib/pEp,share/pEp,include/pEp}

# libpEpAdapter
cp -a ${INSTPREFIX}/lib/libpEpAdapter.a "$D"/lib
cp -av ${INSTPREFIX}/include/pEp/{status_to_string.hh,message_cache.hh,call_with_lock.hh,Adapter.hh,callback_dispatcher.hh,locked_queue.hh,slurp.hh,constant_time_algo.hh,Semaphore.hh,pc_container.hh,passphrase_cache.hh,pEpLog.hh,Adapter.hxx,passphrase_cache.hxx} "$D"/include/pEp


# versions
cp -a ${INSTPREFIX}/*.ver "$D"

find "$D"/lib -maxdepth 1 -type f -print -exec patchelf --set-rpath '$ORIGIN/pEp:$ORIGIN' {} \;
find "$D"/lib/pEp         -type f -print -exec patchelf --set-rpath '$ORIGIN' {} \;
find "$D"/bin -type f -print -exec patchelf --set-rpath '$ORIGIN/../lib/pEp:$ORIGIN/../lib' {} \;

ls -lh "$D"/*
du -sch "$D"
