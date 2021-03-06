/* This file is part of VoltDB.
 * Copyright (C) 2008-2014 VoltDB Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with VoltDB.  If not, see <http://www.gnu.org/licenses/>.
 */

package org.voltcore.utils;

import org.voltcore.utils.DBBPool.BBContainer;

/**
 * Utility class for accessing a variety of com.sun.misc.Unsafe stuff
 */
public final class Bits {

    public static final sun.misc.Unsafe unsafe;

    private static sun.misc.Unsafe getUnsafe() {
        try {
            return sun.misc.Unsafe.getUnsafe();
        } catch (SecurityException se) {
            try {
                return java.security.AccessController.doPrivileged
                        (new java.security
                                .PrivilegedExceptionAction<sun.misc.Unsafe>() {
                            public sun.misc.Unsafe run() throws Exception {
                                java.lang.reflect.Field f = sun.misc
                                        .Unsafe.class.getDeclaredField("theUnsafe");
                                f.setAccessible(true);
                                return (sun.misc.Unsafe) f.get(null);
                            }});
            } catch (java.security.PrivilegedActionException e) {
                throw new RuntimeException("Could not initialize intrinsics",
                        e.getCause());
            }
        }
    }

    private static int PAGE_SIZE = -1;

    static {
        sun.misc.Unsafe unsafeTemp = null;
        try {
            unsafeTemp = getUnsafe();
        } catch (Exception e) {
            e.printStackTrace();
        }
        unsafe = unsafeTemp;
    }

    public static int pageSize() {
        if (PAGE_SIZE == -1) {
            PAGE_SIZE = unsafe.pageSize();
        }
        return PAGE_SIZE;
    }

    public static int numPages(int size) {
        return (size + pageSize()  - 1) / pageSize();
    }

    //Target for storing the checksum to prevent dead code elimination
    private static byte unused;

    public static void readEveryPage(BBContainer cont) {
        long address = cont.address();
        //Make address page aligned
        final int offset = (int)(address % Bits.pageSize());
        address -= offset;
        final int numPages = Bits.numPages(cont.b().remaining() + offset);
        byte checksum = 0;
        for (int ii = 0; ii < numPages; ii++) {
            checksum ^= Bits.unsafe.getByte(address);
            address += checksum;
        }
        //This store will never actually occur, but the compiler doesn't care
        //for the purposes of dead code elimination
        if (unused != 0) {
            unused = checksum;
        }
    }
}
