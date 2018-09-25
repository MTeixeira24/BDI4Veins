/**
 * @cond LICENSE
 * ######################################################################################
 * # LGPL License                                                                       #
 * #                                                                                    #
 * # This file is part of LightVoting by Sophie Dennisen.                               #
 * # Copyright (c) 2017, Sophie Dennisen (sophie.dennisen@tu-clausthal.de)              #
 * # This program is free software: you can redistribute it and/or modify               #
 * # it under the terms of the GNU Lesser General Public License as                     #
 * # published by the Free Software Foundation, either version 3 of the                 #
 * # License, or (at your option) any later version.                                    #
 * #                                                                                    #
 * # This program is distributed in the hope that it will be useful,                    #
 * # but WITHOUT ANY WARRANTY; without even the implied warranty of                     #
 * # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                      #
 * # GNU Lesser General Public License for more details.                                #
 * #                                                                                    #
 * # You should have received a copy of the GNU Lesser General Public License           #
 * # along with this program. If not, see http://www.gnu.org/licenses/                  #
 * ######################################################################################
 * @endcond
 */

package jasonveins.lightvoting.simulation.combinations;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Created by sophie on 08.02.17.
 * uses http://stackoverflow.com/questions/127704/algorithm-to-return-all-combinations-of-k-elements-from-n
 *
 */

public class CCombination extends Object
{

    private List<int[]> m_resultList = new ArrayList<>();

    public List<int[]> getResultList()
    {
        return new ArrayList<>( m_resultList );
    }

    /**
     * compute possible positions of the ones in the committee
     * @param p_arr int[] array
     * @param p_len number of ones
     * @param p_startPosition start position for searching combinations
     * @param p_result position array
     */

    public void combinations( final int[] p_arr, final int p_len, final int p_startPosition, final int[] p_result )
    {
        if ( p_len == 0 )
        {
            m_resultList.add( Arrays.copyOf( p_result, p_result.length ) );
            return;
        }

        for ( int i = p_startPosition; i <= p_arr.length - p_len; i++ )
        {
            p_result[p_result.length - p_len] = p_arr[i];
            this.combinations( p_arr, p_len - 1, i + 1, p_result );
        }
    }

}
