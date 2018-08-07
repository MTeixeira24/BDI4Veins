/*
 * @cond LICENSE
 * ######################################################################################
 * # LGPL License                                                                       #
 * #                                                                                    #
 * # This file is part of the LightJason                                                #
 * # Copyright (c) 2015-16, LightJason (info@lightjason.org)                            #
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

package jasonveins.omnet;

import org.lightjason.agentspeak.common.CCommon;
import org.lightjason.agentspeak.generator.IBaseAgentGenerator;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.io.InputStream;
import java.util.stream.Collectors;
import java.util.stream.Stream;


/* agent generator to create agents */
public final class ManagerGenerator extends IBaseAgentGenerator<ManagerAgent>
{
    private AgentManager agentManager;
    /**
     * @param p_stream ASL code as any stream e.g. FileInputStream
     */
    public ManagerGenerator( @Nonnull final InputStream p_stream, AgentManager m_am) throws Exception
    {
        super(
                // input ASL stream
                p_stream,
                Stream.concat(
                        CCommon.actionsFromPackage(),
                        CCommon.actionsFromAgentClass( ManagerAgent.class )
                ).collect(Collectors.toSet()),
                new CVariableBuilder()
        );
        agentManager = m_am;
    }

    /**
     * generator method of the agent
     *
     * @param p_data any data which can be put from outside to the generator method
     * @return returns an agent
     */
    @Nullable
    @Override
    public final ManagerAgent generatesingle( @Nullable final Object... p_data )
    {
        final ManagerAgent agent = new ManagerAgent( m_configuration, agentManager,(int)p_data[0]);
        return agent;
    }
}
