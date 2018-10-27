//
// Copyright (C) 2015 David Eckhoff <david.eckhoff@fau.de>
//                    Christoph Sommer <sommer@ccs-labs.org>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "NakagamiFadingImproved.h"

#define M_CLOSE 1.5
#define M_FAR 0.75
#define DIS_THRESHOLD 80


using namespace Veins;

/**
 * Simple Nakagami-m fading (based on a constant factor across all time and frequencies).
 */
void NakagamiFadingImproved::filterSignal(Signal* signal, const Coord& senderPos, const Coord& receiverPos)
{

    // get average TX power
    // FIXME: really use average power (instead of max)
    double sendPower_mW = signal->getRelativeMax();

    // get m value
    double m = this->m;
    {
        const Coord senderPos2D(senderPos.x, senderPos.y);
        const Coord receiverPos2D(receiverPos.x, receiverPos.y);
        double d = senderPos2D.distance(receiverPos2D);
        if (!constM) {
            m = -1*(0.69*log(d)) + 4.929;
        }
    }

    // calculate average RX power
    double recvPower_mW = (RNGCONTEXT gamma_d(m, sendPower_mW / 1000 / m)) * 1000.0;
    if (recvPower_mW > sendPower_mW) {
        recvPower_mW = sendPower_mW;
    }

    // infer average attenuation
    double factor = recvPower_mW / sendPower_mW;

    signal->addUniformAttenuation(factor);
}
