FROM ubuntu:16.04

RUN apt-get update

RUN apt-get install -y \
  git \
  wget \
  vim \
  build-essential \
  clang \
  bison \
  flex \
  perl \
  tcl-dev \
  tk-dev \
  libxml2-dev \
  zlib1g-dev \
  default-jre \
  graphviz \
  libwebkitgtk-1.0-0 \
  xvfb
	
RUN apt-get install -y \
  qt5-default \
  qt5-qmake \
  qtbase5-dev \
  openscenegraph \
  libopenscenegraph-dev \
  openscenegraph-plugin-osgearth \
  osgearth \
  osgearth-data \
  libosgearth-dev

RUN wget --header="Accept: text/html" \
         --user-agent="Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:61.0) Gecko/20100101 Firefox/61.0" \
         --referer="https://omnetpp.org/omnetpp/summary/30-omnet-releases/2331-omnetpp-5-4-1-linux" \
         --output-document=omnetpp-5.4.1-src.tgz \
         https://omnetpp.org/omnetpp/send/30-omnet-releases/2331-omnetpp-5-4-1-linux

COPY ./ /usr/share/src/
WORKDIR /usr/share/src/

RUN cd src/ \
	opp_makemake -f --deep -KVEINS_PROJ=$VEINS_HOME -I$\(VEINS_PROJ\)/src -L$\(VEINS_PROJ\)/src -lveins$\(D\) \
	make MODE=debug
