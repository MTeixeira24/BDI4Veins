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

package jasonveins.lightvoting.simulation.agent;

import cern.colt.Arrays;
import cern.colt.bitvector.BitVector;
import com.google.common.util.concurrent.AtomicDoubleArray;
import org.lightjason.agentspeak.action.binding.IAgentAction;
import org.lightjason.agentspeak.action.binding.IAgentActionFilter;
import org.lightjason.agentspeak.action.binding.IAgentActionName;
import org.lightjason.agentspeak.agent.IBaseAgent;
import org.lightjason.agentspeak.common.CCommon;
import org.lightjason.agentspeak.configuration.IAgentConfiguration;
import org.lightjason.agentspeak.generator.IBaseAgentGenerator;
import org.lightjason.agentspeak.language.CLiteral;
import org.lightjason.agentspeak.language.CRawTerm;
import org.lightjason.agentspeak.language.ILiteral;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.CTrigger;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.ITrigger;
//import org.lightjason.agentspeak.language.score.IAggregation;
import jasonveins.lightvoting.simulation.action.message.CSend;
import jasonveins.lightvoting.simulation.constants.CVariableBuilder;
import jasonveins.lightvoting.simulation.environment.CEnvironment;
import jasonveins.lightvoting.simulation.environment.CGroup;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.io.InputStream;
import java.text.MessageFormat;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.concurrent.atomic.AtomicIntegerArray;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.AtomicReference;
import java.util.stream.Collectors;
import java.util.stream.Stream;


/**
 * BDI agent with voting capabilities.
 * re-used code from http://stackoverflow.com/questions/8119366/sorting-hashmap-by-values
 */
// annotation to mark the class that actions are inside
@IAgentAction
public final class CVotingAgent extends IBaseAgent<CVotingAgent>
{

    /**
     * name of the agent
     */
    private final String m_name;

    /**
     * environment
     */
    private final CEnvironment m_environment;

    /**
     * associated chair agent;
     */
    private CChairAgent m_chair;

    /**
     * agent's vote
     */

    private AtomicIntegerArray m_vote;

    /**
     * number of alternatives that the agent is supposed to vote on
     */
    private final int m_altNum;

    /**
     * agent's preferences
     */
    private final AtomicDoubleArray m_atomicPrefValues;

    /**
     * grouping algorithm: "RANDOM" or "COORDINATED"
     */
    private String m_grouping;

    /**
     * variable indicating if agent already submitted its vote
     */

    private boolean m_voted;

    /**
     * threshold for joining a group in the case of coordinated grouping
     */
    private Integer m_joinThreshold;
    private final BitVector m_bitVote;
    private final String m_fileName;

    /**
     * constructor of the agent
     * @param p_name name of the agent
     * @param p_configuration agent configuration of the agent generator
     * @param p_chairagent corresponding chair agent
     * @param p_environment environment reference
     * @param p_altNum number of alternatives
     * @param p_grouping grouping algorithm
     * @param p_fileName h5 file
     */

    public CVotingAgent( final String p_name, final IAgentConfiguration<CVotingAgent> p_configuration, final IBaseAgent<CChairAgent> p_chairagent,
                         final CEnvironment p_environment,
                         final int p_altNum,
                         final String p_grouping,
                         final String p_fileName
    )
    {
        super( p_configuration );
        m_name = p_name;
        m_environment = p_environment;

        m_chair = (CChairAgent) p_chairagent;
        m_storage.put( "chair", p_chairagent.raw() );

        m_beliefbase.add(
            CLiteral.from(
                "chair",
                CRawTerm.from( p_chairagent )
            )
        );

        // sleep chair, Long.MAX_VALUE -> inf
        p_chairagent.sleep( Long.MAX_VALUE );

        m_altNum = p_altNum;

        //Generate random preferences as an Atomic Double Array
        m_atomicPrefValues = this.generatePreferences( m_altNum );
        //Integer Array with the agents votes
        m_vote = this.convertPreferences( m_atomicPrefValues );
        //Bit vector with the agents votes
        m_bitVote = this.convertPreferencesToBits( m_atomicPrefValues );
        m_voted = false;
        m_joinThreshold = 5;
        m_grouping = p_grouping;
        m_fileName = p_fileName;
    }

    // overload agent-cycle
    @Override
    public final CVotingAgent call() throws Exception
    {
        // run default cycle
        return super.call();
    }

    // public methods

    /**
     * Get agent's name
     *
     * @return name of agent
     */
    public final String name()
    {
        return m_name;
    }

    /**
     * get associated chair agent
     *
     * @return chair agent
     */
    public CChairAgent getChair()
    {
        return m_chair;
    }

    public AtomicIntegerArray getVote()
    {
        return m_vote;
    }

    public BitVector getBitVote()
    {
        return m_bitVote;
    }

    // agent actions

    @IAgentActionFilter
    @IAgentActionName( name = "perceive/env" )
    private void perceiveEnv()
    {
        this.beliefbase().add( m_environment.literal( this ) );
        System.out.println( this.name() + " perceived environment " );
    }

    @IAgentActionFilter
    @IAgentActionName( name = "join/group" )
    private void joinGroup()
    {
        if ( "RANDOM".equals( m_grouping ) )
            this.joinGroupRandom();

        if ( "COORDINATED".equals( m_grouping ) )
            this.joinGroupCoordinated();
    }

    @IAgentActionFilter
    @IAgentActionName( name = "submit/vote" )
    private void submitVote( final CChairAgent p_chairAgent )
    {
        if ( m_voted )
            return;

        p_chairAgent.trigger(
            CTrigger.from(
                ITrigger.EType.ADDGOAL,
                CLiteral.from(
                    "vote/received",
                    CRawTerm.from( this.name() ),
                    CRawTerm.from( this.getBitVote() )
                )
            )
        );

        m_voted = true;

    }

    @IAgentActionFilter
    @IAgentActionName( name = "submit/dissatisfaction" )
    private void submitDiss( final CChairAgent p_chairAgent, final Integer p_iteration, final BitVector p_result ) throws InterruptedException
    {
        p_chairAgent.trigger(
            CTrigger.from(
                ITrigger.EType.ADDGOAL,
                CLiteral.from(
                    "diss/received",
                    CRawTerm.from( this.name() ),
                    CRawTerm.from( this.computeDissBV( p_result ) ),
                    CRawTerm.from( p_iteration )
                )
            )
        );
    }

    private Double computeDissBV( final BitVector p_result )
    {
        double l_diss = 0;

        for ( int i = 0; i < p_result.size(); i++ )
        {
            if ( p_result.get( i ) )
                l_diss = l_diss + ( 1 - m_atomicPrefValues.get( i ) );
        }
        return l_diss;
    }

    // private methods

    /**
     * Generates random preferences on each voting option for the agent
     * @param p_altNum The number of alternatives
     * @return An array of size p_altNum of real numbers between 0 and 1 outputted from a sigmoid function
     * representing the preference on each alternative
     */
    private AtomicDoubleArray generatePreferences( final int p_altNum )
    {
        final Random l_random = new Random();
        final double[] l_prefValues = new double[p_altNum];
        for ( int i = 0; i < p_altNum; i++ )
            l_prefValues[i] = this.sigmoidValue( l_random.nextDouble() - 0.5 );
        System.out.println(m_name + " Preference Values: " + Arrays.toString( l_prefValues ) );
        return new AtomicDoubleArray( l_prefValues );
    }

    private double sigmoidValue( double p_var )
    {
        return 1 / ( 1 + Math.pow( Math.E, -1 * p_var ) );
    }

    /**
     * Equal functionality to convertPreferencesToBits but returns an Integer array instead of a Bit Vector. Best to
     * overload the function to return a non repeating sequence of integer preferences
     * @param p_atomicPrefValues Preference values to each alternative
     * @return Integer array with the vote for each alternative
     *
     */
    private AtomicIntegerArray convertPreferences( final AtomicDoubleArray p_atomicPrefValues )
    {
        final int[] l_voteValues = new int[m_altNum];
        for ( int i = 0; i < m_altNum; i++ )
            if ( p_atomicPrefValues.get( i ) > 0.5 )
                l_voteValues[i] = 1;
            else
                l_voteValues[i] = 0;
        System.out.println( "Vote: " + Arrays.toString( l_voteValues ) );
        return new AtomicIntegerArray( l_voteValues );
    }

    /**
     * Creates a BitVector composed of the agents votes
     * @param p_atomicPrefValues The array containing the agents preferences to each alternative
     * @return BitVector where index of vector is the alternative and elements the vote (1 if preference > 0.5, 0 other
     * wise)
     */
    private BitVector convertPreferencesToBits( final AtomicDoubleArray p_atomicPrefValues )
    {
        final BitVector l_voteValues = new BitVector( m_altNum );
        for ( int i = 0; i < m_altNum; i++ )
            if ( p_atomicPrefValues.get( i ) > 0.5 )
                l_voteValues.put( i, true );
            else
                l_voteValues.put( i, false );
        System.out.println(m_name + " Vote as BitVector: " + l_voteValues  );
        return l_voteValues;
    }


    private List<CGroup> determineActiveGroups()
    {
        final AtomicReference<List<CGroup>> l_groupList = new AtomicReference<>();

        m_beliefbase.beliefbase().literal( "groups" ).stream().forEach( i ->
            l_groupList.set( ( (ILiteral) i ).values().findFirst().get().raw() ) );

        final List<CGroup> l_activeGroups = new LinkedList<>();

        for ( int i = 0; i < l_groupList.get().size(); i++ )
            if ( l_groupList.get().get( i ).open() )
            {
                l_activeGroups.add( l_groupList.get().get( i ) );
            }

        return l_activeGroups;
    }

    private void openNewGroup()
    {
        final CGroup l_group;

        if ( "RANDOM".equals( m_grouping ) )
            l_group = m_environment.openNewGroupRandom( this );

        else
            l_group = m_environment.openNewGroupCoordinated( this );

        this.beliefbase().add( l_group.literal( this ) );
        System.out.println( m_name + " opened new group " + l_group );
    }

    private void joinGroupRandom()
    {

        final List<CGroup> l_activeGroups = this.determineActiveGroups();

        if ( l_activeGroups.isEmpty() )
        {
            this.openNewGroup();
            return;
        }

        final Random l_rand = new Random();

        final CGroup l_randomGroup = l_activeGroups.get( l_rand.nextInt( l_activeGroups.size() ) );
        m_environment.addAgentRandom( l_randomGroup, this );
        this.beliefbase().add( l_randomGroup.literal( this ) );

    }

    private void joinGroupCoordinated()
    {
        System.out.println( "join group according to coordinated grouping algorithm" );

        final List<CGroup> l_activeGroups = this.determineActiveGroups();

        if ( l_activeGroups.isEmpty() )
        {
            this.openNewGroup();
            return;
        }

        else
            this.determineGroupCoordinated( l_activeGroups );
    }

    private void determineGroupCoordinated( final List<CGroup> p_activeGroups )
    {
        final CGroup l_group;
        // choose group to join
        final Map<CGroup, Integer> l_groupDistances = new HashMap<>();
        final BitVector l_vote = this.getBitVote();
        System.out.println( "Vote: " + l_vote );
        //Iterative over all active groups
        for ( int i = 0; i < p_activeGroups.size(); i++ )
        {
            //Get the result of the election in the group
            final BitVector l_com =  p_activeGroups.get( i ).result();

            System.out.println( "Vote: " + l_vote );
            System.out.println( "Committee: " + l_com );
            //XOR to find the number of different bits i.e. how many alternatives passed different to what I wanted?
            l_com.xor( l_vote );
            final int l_HD = l_com.cardinality();
            System.out.println( "Hamming distance: " + l_HD );
            l_groupDistances.put( p_activeGroups.get( i ), l_HD );
        }
        //TODO: Optimize this. It appears that the below two lines attempt to find the group with the lowest hamming distance
        final Map l_sortedDistances = this.sortMapDESC( l_groupDistances );
        final Map.Entry<CGroup, Integer> l_entry = (Map.Entry<CGroup, Integer>) l_sortedDistances.entrySet().iterator().next();
        l_group = l_entry.getKey();

        // if Hamming distance is above the threshold, do not join the chair but create a new group
        if ( l_entry.getValue() > m_joinThreshold )
        {
            this.openNewGroup();
            return;
        }
        m_environment.addAgentCoordinated( l_group, this );
        this.beliefbase().add( l_group.literal( this ) );
        System.out.println( this.name() + " joins group " + l_group );
    }

    /**
     * compute dissatisfaction of voter with given committee
     *
     * @param p_resultValues committee
     * @return dissatisfaction with committee
     */

    private double computeDiss( final int[] p_resultValues )
    {
        double l_diss = 0;

        for ( int i = 0; i < p_resultValues.length; i++ )
        {
            if ( p_resultValues[i] == 1 )
                l_diss = l_diss + ( 1 - m_atomicPrefValues.get( i ) );
        }
        return l_diss;
    }



    private Map sortMapDESC( final Map<CGroup, Integer> p_valuesMap )
    {
        final List<Map.Entry<CGroup, Integer>> l_list = new LinkedList<>( p_valuesMap.entrySet() );

        /* Sorting the list based on values in descending order */

        Collections.sort( l_list, ( p_first, p_second ) ->
            p_second.getValue().compareTo( p_first.getValue() ) );

        /* Maintaining insertion order with the help of LinkedList */

        final Map<CGroup, Integer> l_sortedMap = new LinkedHashMap<>();
        for ( final Map.Entry<CGroup, Integer> l_entry : l_list )
        {
            l_sortedMap.put( l_entry.getKey(), l_entry.getValue() );
        }

        return l_sortedMap;
    }


}


