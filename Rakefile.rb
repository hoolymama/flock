require 'flak'

# need this for any rake > 0.8.7
include Rake::DSL

verbose(true)

root = File.dirname(__FILE__)

# load user plugins (may deprecate)
dir = "plugins"
$LOAD_PATH.unshift(dir)
Dir[ File.join(dir, "*.rb") ].each {|file| require File.basename(file,".rb") }

# create targets
FileList.new('**/tool.yml','project.yml').each do |path|
    Flak::Target.new(root, path).build
end

