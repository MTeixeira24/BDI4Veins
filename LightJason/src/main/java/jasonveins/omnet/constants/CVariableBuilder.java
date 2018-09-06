package jasonveins.omnet.constants;

import jasonveins.omnet.agent.IVehicleAgent;
import org.lightjason.agentspeak.agent.IAgent;
import org.lightjason.agentspeak.language.execution.IVariableBuilder;
import org.lightjason.agentspeak.language.instantiable.IInstantiable;
import org.lightjason.agentspeak.language.variable.CConstant;
import org.lightjason.agentspeak.language.variable.IVariable;

import javax.annotation.Nonnull;
import java.text.MessageFormat;
import java.util.stream.Stream;


/**
 * variable build for setting a name
 */
public final class CVariableBuilder implements IVariableBuilder
{
    @Nonnull
    @Override
    public final Stream<IVariable<?>> apply( @Nonnull final IAgent<?> p_agent, @Nonnull final IInstantiable p_runningcontext )
    {
        return Stream.of(
                new CConstant<>( "MyName", MessageFormat.format( "{0}", p_agent.<IVehicleAgent>raw().id() ) ),
                new CConstant<>("MyType",  p_agent.<IVehicleAgent>raw().vType() )
        );

}

    }