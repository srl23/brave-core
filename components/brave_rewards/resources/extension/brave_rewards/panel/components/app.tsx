/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

import { WithThemeVariables } from '../../../../shared/components/with_theme_variables'
import { LegacyThemeProvider } from './legacy_theme_provider'

export function App () {
  return (
    <WithThemeVariables>
      <LegacyThemeProvider>
        <div>
          Hello world
        </div>
      </LegacyThemeProvider>
    </WithThemeVariables>
  )
}
