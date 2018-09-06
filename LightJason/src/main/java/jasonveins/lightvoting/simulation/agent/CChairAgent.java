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

import cern.colt.bitvector.BitVector;
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
import jasonveins.lightvoting.simulation.environment.CEnvironment;
import jasonveins.lightvoting.simulation.environment.CGroup;
import jasonveins.lightvoting.simulation.rule.CMinisumApproval;

import java.io.InputStream;
import java.text.MessageFormat;
import java.util.Collection;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.atomic.AtomicIntegerArray;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.AtomicReference;
import java.util.stream.Collectors;
import java.util.stream.Stream;


/**
 * Created by sophie on 21.02.17.
 */

// annotation to mark the class that actions are inside
@IAgentAction
public final class CChairAgent extends IBaseAgent<CChairAgent>
{

    /**
     * name of chair
     */
    private final String m_name;

    /**
     * environment
     */

    private final CEnvironment m_environment;
    private List<CVotingAgent> m_agents;

    /**
     * grouping algorithm: "RANDOM" or "COORDINATED"
     */
    private String m_grouping;

    private int m_iteration;
    private String m_protocol;
    private List<Double> m_dissList;
    private List<CVotingAgent> m_dissVoters;
    private double m_dissThreshold = 1.1;
    private boolean m_iterative;
    private List<BitVector> m_bitVotes;
    private final String m_fileName;

    /**
     * constructor of the agent
     * @param p_configuration agent configuration of the agent generator
     * @param p_grouping grouping algorithm
     * @param p_protocol voting protocol
     * @param p_fileName h5 file
     */


    public CChairAgent( final String p_name, final IAgentConfiguration<CChairAgent> p_configuration, final CEnvironment p_environment, final String p_grouping,
                        final String p_protocol,
                        final String p_fileName
    )
    {
        super( p_configuration );
        m_name = p_name;
        m_environment = p_environment;
        m_bitVotes = Collections.synchronizedList( new LinkedList<>() );
        m_dissList = Collections.synchronizedList( new LinkedList<>() );
        m_dissVoters = Collections.synchronizedList( new LinkedList<>() );
        m_grouping = p_grouping;
        m_protocol = p_protocol;
        m_iteration = 0;
        m_agents = Collections.synchronizedList( new LinkedList<>() );
        m_iterative = false;
        m_fileName = p_fileName;
    }

    // overload agent-cycle
    @Override
    public final CChairAgent call() throws Exception
    {
        // run default cycle
        return super.call();
    }

    // public methods

    public String name()
    {
        return m_name;
    }

    // agent actions

    /**
     * perceive group
     */
    @IAgentActionFilter
    @IAgentActionName( name = "perceive/group" )
    /**
     * add literal for group of chair agent if it exists
     */
    public void perceiveGroup()
    {
        if (  m_environment.detectGroup( this ) != null  )
            this.beliefbase().add( m_environment.detectGroup( this ) );
    }

    /**
     * check conditions
     */
    @IAgentActionFilter
    @IAgentActionName( name = "check/conditions" )
    /**
     * add literal for group of chair agent if it exists
     */
    public void checkConditions()
    {
        final CGroup l_group = this.determineGroup();

        // if conditions for election are fulfilled, trigger goal start/criterion/fulfilled

        final ITrigger l_trigger;

        // if m_iterative is true, we have the case of iterative voting, i.e. we already have the votes
        // we only need to repeat the computation of the result

        if  ( m_iterative && ( l_group.readyForElection() && !( l_group.electionInProgress() ) ) )
        {
            m_iteration++;
            this.computeResult();
            return;
        }


        if ( l_group.readyForElection() && ( !( l_group.electionInProgress() ) ) )
        {
            l_group.startProgress();

            l_trigger = CTrigger.from(
                ITrigger.EType.ADDGOAL,
                CLiteral.from( "start/criterion/fulfilled" )
            );

            this.trigger( l_trigger );
        }
    }

    // private methods

    private CGroup determineGroup()
    {
        final AtomicReference<CGroup> l_groupAtomic = new AtomicReference<>();
        final Collection l_groups = this.beliefbase().beliefbase().literal( "group" );
        l_groups.stream().forEach( i -> l_groupAtomic.set( ( (ILiteral) i ).values().findFirst().get().raw() ) );
        return l_groupAtomic.get();
    }


    /**
     * start election
     */
    @IAgentActionFilter
    @IAgentActionName( name = "start/election" )

    public void startElection()
    {
        final CGroup l_group = this.determineGroup();
        l_group.triggerAgents( this );
    }

    /**
     * store vote
     *
     * @param p_vote vote
     */
    @IAgentActionFilter
    @IAgentActionName( name = "store/vote" )
    public void storeVote( final String p_agentName, final BitVector p_vote )
    {
        final CGroup l_group = this.determineGroup();

        m_agents.add( l_group.determineAgent( p_agentName ) );
        m_bitVotes.add( p_vote );

        if ( m_bitVotes.size() == l_group.size() )
        {

            final ITrigger l_trigger = CTrigger.from(
                ITrigger.EType.ADDGOAL,
                CLiteral.from(
                    "all/votes/received" )

            );

            System.out.println( " CChairAgent.java: all votes received " );

            this.trigger( l_trigger );
        }
    }

    private BitVector toBV( final AtomicIntegerArray p_vote )
    {
        final BitVector l_bitVector = new BitVector( p_vote.length() );
        for ( int i = 0; i < p_vote.length(); i++ )
            if ( p_vote.get( i ) == 1 )
                l_bitVector.put( i, true );
        return l_bitVector;
    }

    /**
     * compute result of election
     */

    @IAgentActionFilter
    @IAgentActionName( name = "compute/result" )

    public void computeResult()
    {
        final CGroup l_group = this.determineGroup();

        final CMinisumApproval l_minisumApproval = new CMinisumApproval();

        final List<String> l_alternatives = new LinkedList<>();

        for ( char l_char : "ABCDEF".toCharArray() )

            l_alternatives.add( String.valueOf( l_char ) );

        System.out.println( " Alternatives: " + l_alternatives );

        System.out.println( " Votes: " + m_bitVotes );

        final BitVector l_comResultBV = l_minisumApproval.applyRuleBV( l_alternatives, m_bitVotes, 3 );

        System.out.println( " Result of election as BV: " + l_comResultBV );

        // set inProgress and readyForElection to false in group
        l_group.reset();

        if ( "BASIC".equals( m_protocol ) )
        {
            this.beliefbase().add( l_group.updateBasic( this, l_comResultBV ) );
        }


        // if grouping is coordinated, reopen group for further voters
        if ( "COORDINATED".equals( m_grouping ) && !l_group.finale() && !m_iterative )
        {
            System.out.println( " reopening group " );
            m_environment.reopen( l_group );
        }

        // for the iterative case, you need to differentiate between the final election and intermediate elections.
        if ( "ITERATIVE".equals( m_protocol ) && ( l_group.finale() ) || m_iterative )
        {
            System.out.println( " Update iterative " );

            this.beliefbase().add( l_group.updateIterative( this,  l_comResultBV, m_iteration ) );
            return;
        }


        if ( "ITERATIVE".equals( m_protocol ) && !l_group.finale() )
        {
            System.out.println( " Update basic " );
            this.beliefbase().add( l_group.updateBasic( this,  l_comResultBV ) );
        }

        // TODO test all cases

    }

    /**
     * store dissatisfaction value
     *
     * @param p_diss dissatisfaction value
     * @param p_iteration iteration number
     */
    @IAgentActionFilter
    @IAgentActionName( name = "store/diss" )

    public void storeDiss( final String p_name, final Double p_diss, final Integer p_iteration )
    {
        final CGroup l_group = this.determineGroup();

        m_dissList.add( p_diss );
        final CVotingAgent l_dissAg = l_group.determineAgent( p_name );
        m_dissVoters.add( l_dissAg );

        System.out.println( "Storing diss " + p_diss );

        if ( m_dissList.size() == l_group.size() )
        {
            final ITrigger l_trigger = CTrigger.from(
                ITrigger.EType.ADDGOAL,
                CLiteral.from(
                    "all/dissValues/received",
                    CRawTerm.from( p_iteration )
                )

            );

            this.trigger( l_trigger );

            System.out.println( p_iteration + " All voters submitted their dissatisfaction value" );
        }
    }

    /**
     * remove most dissatisfied voter
     */
    @IAgentActionFilter
    @IAgentActionName( name = "remove/voter" )
    public void removeVoter()
    {
        final CGroup l_group = this.determineGroup();

        final int l_maxIndex = this.getMaxIndex( m_dissList );
        final double l_max = m_dissList.get( l_maxIndex );
        System.out.println( " max diss is " + l_max );

        if ( l_max > m_dissThreshold )
        {
            System.out.println( " Determining most dissatisfied voter " );
            final CVotingAgent l_maxDissAg = m_dissVoters.get( l_maxIndex );
            System.out.println( " Most dissatisfied voter is " + l_maxDissAg.name() );
            // remove vote of most dissatisfied voter from list
            m_bitVotes.remove( l_maxDissAg.getBitVote() );
            m_dissVoters.remove( l_maxDissAg );
            l_group.remove( l_maxDissAg );

            System.out.println( "Removing " + l_maxDissAg.name() );

            // remove diss Values for next iteration
            m_dissList.clear();

            m_iterative = true;
            l_group.makeReady();

            if ( l_group.size() == 0 )
                System.out.println( " Voter list is empty, we are done " );

            return;
        }

        System.out.println( " No dissatisfied voter left, we are done " );
    }

    private int getMaxIndex( final List<Double> p_dissValues )
    {
        int l_maxIndex = 0;
        for ( int i = 0; i < p_dissValues.size(); i++ )
        {
            if ( p_dissValues.get( i ) > p_dissValues.get( l_maxIndex ) )
            {
                System.out.println( " changed max index to " + i + " diss: " + p_dissValues.get( i ) );
                l_maxIndex = i;
            }
        }
        return l_maxIndex;
    }

    /**
     * Class CChairAgentGenerator
     */

    public static final class CChairAgentGenerator extends IBaseAgentGenerator<CChairAgent>
    {

        /**
         * environment
         */
        private final CEnvironment m_environment;

        /**
         * Current free agent id, needs to be thread-safe, therefore using AtomicLong.
         */
        private final AtomicLong m_agentcounter = new AtomicLong();

        private final String m_grouping;
        private String m_protocol;
        private final String m_fileName;

        /**
         * constructor of the generator
         * @param p_stream ASL code as any stream e.g. FileInputStream
         * @param p_grouping grouping algorithm
         * @param p_protocol voting protocol
         * @param p_fileName h5 file
         * @throws Exception Thrown if something goes wrong while generating agents.
         */
        public CChairAgentGenerator( final InputStream p_stream, final CEnvironment p_environment, final String p_grouping, final String p_protocol,
                                     final String p_fileName
        ) throws Exception
        {
            super(
                // input ASL stream
                p_stream,

                // a set with all possible actions for the agent
                Stream.concat(
                    // we use all build-in actions of LightJason
                    CCommon.actionsFromPackage(),
                    Stream.concat(
                        // use the actions which are defined inside the agent class
                        CCommon.actionsFromAgentClass( CChairAgent.class ),
                        // add VotingAgent related external actions
                        Stream.of(

                        )
                    )
                    // build the set with a collector
                ).collect( Collectors.toSet() )/*,

                // aggregation function for the optimisation function, here
                // we use an empty function
                IAggregation.EMPTY*/
            );
            m_environment = p_environment;
            m_grouping = p_grouping;
            m_protocol = p_protocol;
            m_fileName = p_fileName;
        }

        /**
         * generator method of the agent
         * @param p_data any data which can be put from outside to the generator method
         * @return returns an agent
         */

        @Override
        public final CChairAgent generatesingle( final Object... p_data )
        {
            final CChairAgent l_chairAgent = new CChairAgent(
                // create a string with the agent name "chair <number>"
                // get the value of the counter first and increment, build the agent
                // name with message format (see Java documentation)
                MessageFormat.format( "chair {0}", m_agentcounter.getAndIncrement() ), m_configuration, m_environment, m_grouping, m_protocol, m_fileName );
            l_chairAgent.sleep( Integer.MAX_VALUE );
            return l_chairAgent;
        }

    }
}
