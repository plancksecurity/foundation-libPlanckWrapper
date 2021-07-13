include ../DEPENDENCIES
export
CURRENT_DISTRO=${CI_DISTRO_TARGET}
IMAGE_NAME=${DOCKER_REGISTRY_HOST}/pep-$(CI_DISTRO_TARGET)-libpepadapter
DOCKERFILE=libpEpAdapter.linux.Dockerfile
PKG_BUILD_IMAGE=${DOCKER_REGISTRY_HOST}/fpm-$(CURRENT_DISTRO)
PKG_INSTALL_PATH=/opt/pEp
PKG_NAME=libpEpAdapter
PKG_DESCRIPTION="p≡p libpEpAdapter Binary RPM Package"
PKG_DEPENDS=pEpEngine
PKG_INSTALL_PATH_STRING="/ /package/lib=${PKG_INSTALL_PATH} /package/include/pEp=${PKG_INSTALL_PATH}/include"
IS_TAGGED=${TAGGED_BUILD}
ifeq ($(IS_TAGGED), true)
# $CI_COMMIT_TAG is a predefined environment variable from Gitlab
	LIBPEPADAPTER_VERSION=${CI_COMMIT_TAG}
else
	LIBPEPADAPTER_VERSION=$(shell git rev-parse --short=8 HEAD)
endif
all: build

build:
	-docker pull $(IMAGE_NAME):latest
	cd ../ && docker build --build-arg CURRENT_DISTRO=$(CURRENT_DISTRO) \
		  --build-arg DOCKER_REGISTRY_HOST=${DOCKER_REGISTRY_HOST} \
		  --build-arg LIBPEPADAPTER_VERSION=$(LIBPEPADAPTER_VERSION) \
		  --cache-from $(IMAGE_NAME):latest \
		  --tag=$(IMAGE_NAME):$(LIBPEPADAPTER_VERSION) \
		  --tag=$(IMAGE_NAME):latest \
		  -f scripts/common/$(DOCKERFILE) .
	docker push $(IMAGE_NAME):${LIBPEPADAPTER_VERSION}
	docker push $(IMAGE_NAME):latest

rpm:
	-docker pull $(PKG_BUILD_IMAGE)-libpepadapter:latest
	@docker build --build-arg CURRENT_DISTRO=$(CURRENT_DISTRO) \
		--build-arg LIBPEPADAPTER_VERSION=$(LIBPEPADAPTER_VERSION) \
		--build-arg DOCKER_REGISTRY_HOST=${DOCKER_REGISTRY_HOST} \
		--build-arg PKG_INSTALL_PATH=$(PKG_INSTALL_PATH) \
		--cache-from $(PKG_BUILD_IMAGE)-engine:latest \
		--tag=$(PKG_BUILD_IMAGE)-libpepadapter:$(LIBPEPADAPTER_VERSION) \
		--tag=$(PKG_BUILD_IMAGE)-libpepadapter:latest \
		centos8/packages/rpm
	@docker push $(PKG_BUILD_IMAGE)-libpepadapter:$(LIBPEPADAPTER_VERSION)
	@docker push $(PKG_BUILD_IMAGE)-libpepadapter:latest
	@docker run -e LIBPEPADAPTER_VERSION=$(LIBPEPADAPTER_VERSION) \
		-e PEP_MACHINE_DIR=$(PEP_MACHINE_DIR) \
		-e PKG_VERSION=$(LIBPEPADAPTER_VERSION) \
		-e PKG_INSTALL_PATH=$(PKG_INSTALL_PATH) \
		-e PKG_NAME=$(PKG_NAME) \
		-e PKG_DESCRIPTION=$(PKG_DESCRIPTION) \
		-e PKG_DEPENDS=$(PKG_DEPENDS) \
		-e PKG_INSTALL_PATH_STRING=$(PKG_INSTALL_PATH_STRING) \
		--rm -v $(shell pwd)/centos8/packages/rpm/create-libpepadapter-rpm.sh:/usr/bin/create-rpm.sh:ro  \
		-v $(shell pwd)/out:/out \
		-w / $(PKG_BUILD_IMAGE)-libpepadapter:${LIBPEPADAPTER_VERSION} \
		/usr/bin/create-rpm.sh
