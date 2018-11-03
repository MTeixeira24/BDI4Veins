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

package jasonveins.lightvoting.simulation.rule;

import cern.colt.bitvector.BitVector;
import jasonveins.lightvoting.simulation.combinations.CCombination;

import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;


/**
 * Created by sophie on 10.01.17.
 * re-used code from http://stackoverflow.com/questions/8119366/sorting-hashmap-by-values
 */
public class CMinimaxApproval
{

     /***
         * compute the winning committee according to Minimax Approval
         *
         * @param p_alternatives available alternatives
         * @param p_votes submitted votes
         * @param p_comSize size of committee to be elected
         * @return elected committee
         */

    public BitVector applyRuleBV( final List<String> p_alternatives, final List<BitVector> p_votes, final int p_comSize )
    {
        /* compute all possible committees, i.e. all {0,1}^m vectors with exactly k ones */

        final List<BitVector> l_bitCommittees = this.computeBitComittees( p_alternatives.size(), p_comSize );
        System.out.println( l_bitCommittees );

        /* Hashmap for storing the maximal hamming distance to any vote for all committees */

        Map<Integer, Integer> l_maxMap = new HashMap<Integer, Integer>();

        for ( int i = 0; i < l_bitCommittees.size(); i++ )
        {
            /* Key: Committee ID, Value: maximal Hamming distance to any voter */
            l_maxMap.put(  i, this.determineMaxHDBV( p_votes, l_bitCommittees.get( i ), p_alternatives.size() ) );
        }

        l_maxMap = this.sortMapASC( l_maxMap );

        return l_bitCommittees.get( l_maxMap.entrySet().iterator().next().getKey() );

    }

    /**
     * compute possible committees
     * @param p_altNum number of alternatives
     * @param p_comSize committee size
     * @return possible committees
     */

    private List<BitVector> computeBitComittees( final int p_altNum, final int p_comSize )
    {
        final CCombination l_combination = new CCombination();
        final int[] l_arr = new int[p_altNum];

        for ( int i = 0; i < p_altNum; i++ )
            l_arr[i] = i;

        l_combination.combinations( l_arr, p_comSize, 0, new int[p_comSize] );

        final List<int[]> l_resultList = l_combination.getResultList();

        final List<BitVector> l_bitVectors = new LinkedList<>();

        for ( int i = 0; i < l_resultList.size(); i++ )
        {
            final BitVector l_bitVector = new BitVector( p_altNum );

            for ( int j = 0; j < p_comSize; j++ )
            {
                l_bitVector.put( l_resultList.get( i )[j], true );
            }
            l_bitVectors.add( l_bitVector );
        }

        return l_bitVectors;
    }

    private int determineMaxHDBV( final List<BitVector> p_votes, final BitVector p_comVect, final int p_altNum )
    {
        /* compute Hamming distances to all votes and determine the maximum */

        int l_maxHD = -1;

        for ( int i = 0; i < p_votes.size(); i++ )
        {
            final BitVector l_curBitCom = p_comVect.copy();

            l_curBitCom.xor( p_votes.get( i ) );

            final int l_curHD = l_curBitCom.cardinality();

            if ( l_curHD > l_maxHD )
                l_maxHD = l_curHD;
        }
        return l_maxHD;
    }

    /**
     * sort HashMap according to its values in ascending order
     *
     * @param p_valuesMap HashMap with Approval scores
     * @return sorted HashMap
     */


    public Map<Integer, Integer> sortMapASC( final Map<Integer, Integer> p_valuesMap )

    {
        final List<Map.Entry<Integer, Integer>> l_list = new LinkedList<>( p_valuesMap.entrySet() );

        // Sorting the list based on values in ascending order
        Collections.sort( l_list, Comparator.comparing( Map.Entry::getValue ) );

        /* Maintaining insertion order with the help of LinkedList */
        final Map<Integer, Integer> l_sortedMap = new LinkedHashMap<Integer, Integer>();
        for ( final Map.Entry<Integer, Integer> l_entry : l_list )
        {
            l_sortedMap.put( l_entry.getKey(), l_entry.getValue() );
        }

        return l_sortedMap;
    }
}


