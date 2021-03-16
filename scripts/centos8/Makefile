CURRENT_DISTRO=$(shell basename $(shell pwd))
LIBPEPADAPTER_VERSION=$(shell git rev-parse --short=8 HEAD)
IMAGE_NAME=${DOCKER_REGISTRY_HOST}/pep-$(CURRENT_DISTRO)-libpepadapter
DOCKERFILE=libpEpAdapter.$(CURRENT_DISTRO).Dockerfile
all:
	-docker pull $(IMAGE_NAME):latest
	cd ../../ && docker build --build-arg CURRENT_DISTRO=$(CURRENT_DISTRO) \
		      --build-arg DOCKER_REGISTRY_HOST=${DOCKER_REGISTRY_HOST} \
		      --build-arg LIBPEPADAPTER_VERSION=$(LIBPEPADAPTER_VERSION) \
		      --cache-from $(IMAGE_NAME):latest \
		      --tag=$(IMAGE_NAME):$(LIBPEPADAPTER_VERSION) \
		      --tag=$(IMAGE_NAME):latest \
		      -f scripts/${CURRENT_DISTRO}/$(DOCKERFILE) .
	docker push $(IMAGE_NAME):${LIBPEPADAPTER_VERSION}
	docker push $(IMAGE_NAME):latest
