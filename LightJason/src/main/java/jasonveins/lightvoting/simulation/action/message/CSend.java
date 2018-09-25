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

package jasonveins.lightvoting.simulation.action.message;

import org.lightjason.agentspeak.action.IBaseAction;
import org.lightjason.agentspeak.agent.IAgent;
import org.lightjason.agentspeak.common.CPath;
import org.lightjason.agentspeak.common.IPath;
import org.lightjason.agentspeak.language.CLiteral;
import org.lightjason.agentspeak.language.CRawTerm;
import org.lightjason.agentspeak.language.ITerm;
import org.lightjason.agentspeak.language.execution.IContext;
import org.lightjason.agentspeak.language.fuzzy.CFuzzyValue;
import org.lightjason.agentspeak.language.fuzzy.IFuzzyValue;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.CTrigger;
import org.lightjason.agentspeak.language.instantiable.plan.trigger.ITrigger;
import jasonveins.lightvoting.simulation.agent.CVotingAgent;

import javax.annotation.Nonnull;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;


/**
 * Action to send messages for communication.
 * Based on/credits to https://lightjason.github.io/tutorials/communication/
 */
public final class CSend extends IBaseAction
{
    /**
     * thread-safe map for storing name and agent object
     */
    private final Map<String, CVotingAgent> m_agents = new ConcurrentHashMap<>();

    /** Register a new agent.
     * @param p_agent agent object
     * @return object of registered agent
     */
    public final CVotingAgent register( final CVotingAgent p_agent )
    {
        m_agents.put( p_agent.name(), p_agent );
        return p_agent;
    }

    /** Unregisters agent
     * Removes agent from map
     * @param p_agent agent object
     * @return object of unregistered agent
     */
    public final CVotingAgent unregister( final CVotingAgent p_agent )
    {
        m_agents.remove( p_agent.name() );
        return p_agent;
    }

    @Override
    public final IPath name()
    {
        return CPath.from( "message/send" );
    }

    @Override
    public final int minimalArgumentNumber()
    {
        return 2;
    }



    @Override
    public final IFuzzyValue<Boolean> execute( boolean p_parallel, @Nonnull IContext p_context, @Nonnull List<ITerm> p_argument, @Nonnull List<ITerm> p_return )
    {
        /**
         * first parameter of the action is the name of the receiving agent
         */
        final IAgent<?> l_receiver = m_agents.get( p_argument.get( 0 ).<String>raw() );

        // if the agent is it not found, action fails
        if ( l_receiver == null )
            return CFuzzyValue.from( false );

        // create the receiving goal-trigger of the message
        l_receiver.trigger(
                CTrigger.from(
                        ITrigger.EType.ADDGOAL,

                        // create the goal literal "message/receive(M,S)" with M is the message literal
                        // and S the sending agent name
                        CLiteral.from(
                                "message/receive",

                                // message literal
                                CLiteral.from(
                                        "message",

                                        // first argument is the agent name so copy all other arguments to the message literal
                                        p_argument.subList( 1, p_argument.size() ).stream().map( i -> CRawTerm.from( i.raw() ) )
                                ),

                                // name of the sending agent in this the agent which calls the send action is read from
                                // context and translate in the communication agent, the communication agent has got the
                                // method name() to read the agent name
                                CLiteral.from( "from", CRawTerm.from( p_context.agent().<CVotingAgent>raw().name() ) )
                        )

                )
        );

        return CFuzzyValue.from( true );
    }
}
