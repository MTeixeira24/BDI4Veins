FROM mteixeira24/omnetpp:5.4

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


COPY ./ /usr/share/src/
WORKDIR /usr/share/src/

#CMD echo $PATH

#CMD ./src/veinsBDI_dbg ./simulations/simpleSim/omnetpp.ini

RUN cd src/ && opp_makemake -f --deep -KVEINS_PROJ=$VEINS_HOME -I$\(VEINS_PROJ\)/src -L$\(VEINS_PROJ\)/src -lveins$\(D\) && make MODE=debug && cd ../
ENV NEDFILES "../../src/:..:/usr/share/veins/src/veins:."
WORKDIR simulations/simpleSim/

CMD ../../src/src_dbg -u Cmdenv -n $NEDFILES
