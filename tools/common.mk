# Determine relative path of file from project base
# readlink to ensure folder is canonical
# $(1) = File to get relative path of
define relative_path
$(subst $(BASE)/,,$(shell $(READLINK) -m "$(1)"))
endef

RELATIVE_CURDIR = $(call relative_path,$(CURDIR))

# Pretty print the executing command
# $(1) = Command name
# $(2) = Affected file
define print_command
$(VERBOSE)printf "%-7s %s\n" "$(1)" "$(2)"
endef
