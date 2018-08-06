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

package trafficproject;

import org.lightjason.agentspeak.common.CCommon;
import org.lightjason.agentspeak.generator.IBaseAgentGenerator;
import trafficproject.environment.Lane;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.io.InputStream;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.stream.Collectors;
import java.util.stream.Stream;


/* agent generator to create agents */
public final class MyAgentGenerator extends IBaseAgentGenerator<MyAgent>
{
    private final Lane m_lane;
    private AtomicInteger generatedAgents = new AtomicInteger();
    /**
     * @param p_stream ASL code as any stream e.g. FileInputStream
     */
    public MyAgentGenerator( @Nonnull final InputStream p_stream, @Nonnull final Lane p_lane ) throws Exception
    {
        super(
            // input ASL stream
            p_stream,
                Stream.concat(
                        CCommon.actionsFromPackage(),
                        CCommon.actionsFromAgentClass( MyAgent.class )
                ).collect(Collectors.toSet()),
            // we use all build-in actions of LightJason
                new CVariableBuilder()

        );
        m_lane = p_lane;
        generatedAgents.set(0);
    }

    /**
     * generator method of the agent
     *
     * @param p_data any data which can be put from outside to the generator method
     * @return returns an agent
     */
    @Nullable
    @Override
    public final MyAgent generatesingle( @Nullable final Object... p_data )
    {
        int m_id = generatedAgents.getAndAdd(1);
        final MyAgent agent = new MyAgent( m_configuration, 10, m_lane, ((int)(Math.random() * 10)) * 10, m_id );
        return agent;
    }
}
