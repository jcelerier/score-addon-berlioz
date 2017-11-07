#include "score_addon_berlioz.hpp"
#include <score_addon_berlioz_commands_files.hpp>

#include <berlioz/Process.hpp>
#include <berlioz/Executor.hpp>
#include <berlioz/Inspector.hpp>
#include <berlioz/LocalTree.hpp>
#include <berlioz/Layer.hpp>
#include <berlioz/CommandFactory.hpp>
#include <berlioz/ApplicationPlugin.hpp>

#include <score/plugins/customfactory/FactorySetup.hpp>

score_addon_berlioz::score_addon_berlioz()
{

}

score_addon_berlioz::~score_addon_berlioz()
{

}

std::vector<std::unique_ptr<score::InterfaceBase> >
score_addon_berlioz::factories(
        const score::ApplicationContext& ctx,
        const score::InterfaceKey& key) const
{
    return instantiate_factories<
            score::ApplicationContext,
        FW<Process::ProcessModelFactory,
           berlioz::ProcessFactory>,
        FW<Process::LayerFactory,
           berlioz::LayerFactory>,
        FW<Process::InspectorWidgetDelegateFactory,
           berlioz::InspectorFactory>,
        FW<Engine::Execution::ProcessComponentFactory,
           berlioz::ProcessExecutorComponentFactory>,
        FW<Engine::LocalTree::ProcessComponentFactory,
           berlioz::LocalTreeProcessComponentFactory>
    >(ctx, key);
}

std::pair<const CommandGroupKey, CommandGeneratorMap>
score_addon_berlioz::make_commands()
{
    using namespace berlioz;
    std::pair<const CommandGroupKey, CommandGeneratorMap> cmds{
        CommandFactoryName(),
        CommandGeneratorMap{}};

    using Types = TypeList<
#include <score_addon_berlioz_commands.hpp>
      >;

    for_each_type<Types>(score::commands::FactoryInserter{cmds.second});

    return cmds;
}

score::GUIApplicationPlugin*score_addon_berlioz::make_guiApplicationPlugin(const score::GUIApplicationContext& app)
{
  return new berlioz::ApplicationPlugin{app};
}
