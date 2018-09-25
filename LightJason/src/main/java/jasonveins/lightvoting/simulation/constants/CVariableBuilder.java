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

package jasonveins.lightvoting.simulation.constants;

import org.lightjason.agentspeak.agent.IAgent;
import org.lightjason.agentspeak.language.execution.IVariableBuilder;
import org.lightjason.agentspeak.language.instantiable.IInstantiable;
import org.lightjason.agentspeak.language.variable.CConstant;
import org.lightjason.agentspeak.language.variable.IVariable;
import jasonveins.lightvoting.simulation.agent.CVotingAgent;
import jasonveins.lightvoting.simulation.environment.CEnvironment;

import java.text.MessageFormat;
import java.util.stream.Stream;


/**
 * Created by sophie on 21.02.17.
 */

public final class CVariableBuilder implements IVariableBuilder
{

    /**
     * environment reference
     */

    private final CEnvironment m_environment;

    /**
     * constructor
     *
     * @param p_environment environment
     */
    public CVariableBuilder( final CEnvironment p_environment )
    {
        m_environment = p_environment;
    }

    @Override
    public final Stream<IVariable<?>> apply( final IAgent<?> p_agent, final IInstantiable p_runningcontext )
    {
        return Stream.of(
            new CConstant<>( "MyName", MessageFormat.format( "{0}",  p_agent.<CVotingAgent>raw().name() ) )
        );
    }
}
