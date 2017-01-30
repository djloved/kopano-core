"""
Part of the high-level python bindings for Kopano

Copyright 2005 - 2016 Zarafa and its licensors (see LICENSE file for details)
Copyright 2016 - Kopano and its licensors (see LICENSE file for details)
"""

from MAPI.Tags import PR_RULE_NAME, PR_RULE_STATE, ST_ENABLED

from .compat import repr as _repr, fake_unicode as _unicode

from .restriction import Restriction

class Rule(object):
    def __init__(self, mapirow):
        self.mapirow = mapirow
        name, state = mapirow[PR_RULE_NAME], mapirow[PR_RULE_STATE]
        self.name = _unicode(name)
        self.active = bool(state & ST_ENABLED)

    @property
    def restriction(self):
        return Restriction(mapiobj=self.mapirow[PR_RULE_CONDITION])

    def __unicode__(self):
        return u"Rule('%s')" % self.name

    def __repr__(self):
        return _repr(self)
