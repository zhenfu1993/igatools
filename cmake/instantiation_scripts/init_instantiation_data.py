#-+--------------------------------------------------------------------
# Igatools a general purpose Isogeometric analysis library.
# Copyright (C) 2012-2014  by the igatools authors (see authors.txt).
#
# This file is part of the igatools library.
#
# The igatools library is free software: you can use it, redistribute
# it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either
# version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#-+--------------------------------------------------------------------

#TODO: remove ref and phys space variables
#TODO: remove table variable replace by userspaces


# This module is loaded from all instantiation scripts.
# It will read a .txt table created by another script called
# generate_instantiation_table.py

# Removes duplicates of a list while keeping the original order
def unique(seq): 
   checked = []
   for e in seq:
       if e not in checked:
           checked.append(e)
   return checked

# Object to store a row containig the description of a physical space.
class PhysSpaceTableRow:
    # Constructor of the class.
    def __init__(self, arg_list):
        self.dim_ref    = arg_list[0]
        self.range_ref  = arg_list[1]
        self.rank_ref   = arg_list[2]
        self.dim_phys   = arg_list[3]
        self.trans_type = arg_list[4]
        return None



# Object to store different tables with useful
# information used for instantiations.  
#
# They are all generated from the physical spaces tables
# that the user wishes to compile the library for.
class InstantiationInfo:
    # Constructor of the class.
    def __init__(self, filename, max_der_order):
        self.user_table=[] # Spaces that the library is suppussed to be used on
        self.face_table=[] # Spaces that are faces of the user spaces
        self.table = [] #the main data, contains the physical spaces the user 
                        #provides plus the one that are necesary on top
        self.UserRefDims=[]   # the list of the dimension  <d,d,d> of all ref spaces
        self.RefDims=[]   # the list of the dimension  <d,d,d> of all ref spaces       
        self.RefSpaces=[] # all required reference spaces
        self.UserRefSpaces=[]
        self.UserFilteredRefSpaces=[]
        
        self.UserMappingDims=[] #list of <dim, codims>
        self.MappingDims=[] #list of <dim, codims>
        
        self.PushForwards=[]
        self.UserPhysSpaces=[]
        self.PhysSpaces=[]
        self.deriv_order = range(int(max_der_order)+1)
        
        self.ref_dom_dims = [] # list ref domain dimension todo: change to ref_domain
        self.user_ref_dom_dims = []
        self.face_ref_dom_dims = []
        
        self.derivatives=[]  #derivative classes 
        self.read_dimensions_file(filename)
        self.create_RefSpaces()
        self.create_PhysSpaces()
        self.create_ref_dim()
        self.create_Mappings()
        self.create_derivatives()
        return None



    def read_dimensions_file(self, filename):
        '''Reads a text file where each line describes a physical space and 
            genereate the tables '''
        
        file_input = open(filename, 'r')
        user_spaces=[]  
        for i in file_input:
            row = i.strip().split()
            try: # This is for skipping commented lines (starting with #)
                user_spaces.append([int(x) for x in row])
            except:
                pass
        file_input.close()


        for row in user_spaces:
            self.user_table.append(PhysSpaceTableRow(row))
            self.table.append(PhysSpaceTableRow(row))
        
        face_spaces = unique( 
                       [ [sp.dim_ref-1, sp.range_ref, sp.rank_ref,
                          sp.dim_phys, sp.trans_type]
                        for sp in self.user_table ]
                       )    
                 
                               
        for row in face_spaces:
            self.face_table.append(PhysSpaceTableRow(row))
            self.table.append(PhysSpaceTableRow(row))
                    
        unique(self.table)
        return None



    def create_RefSpaces(self):
        ''' Creates a list of Reference spaces '''
        spaces =('BSplineSpace', 'NURBSSpace')
        self.RefDims = unique( ['<%d,%d,%d>' % (x.dim_ref, x.range_ref, x.rank_ref)
                                for x in self.table] )
        self.RefSpaces = ( ['%s%s' % (sp, dims)  
                            for sp in spaces
                            for dims in self.RefDims] )
        self.UserRefDims = unique( ['<%d,%d,%d>' % (x.dim_ref, x.range_ref, x.rank_ref)
                                for x in self.user_table] )
        self.UserRefSpaces = ( ['%s%s' % (sp, dims)  
                            for sp in spaces
                            for dims in self.UserRefDims] )
        temp=[]
        temp = unique( ['<%d,%d,%d>' % (x.dim_ref, x.range_ref, x.rank_ref)
                                for x in self.user_table if x.dim_ref >= x.range_ref] )
        self.UserFilteredRefSpaces = ( ['%s%s' % (sp, dims)  
                                       for sp in spaces
                                       for dims in temp] )
        return None
    
    
    # Mapping<dim_ref, codim>
    def create_Mappings(self):
        ''' Creates a list of mappings '''
        self.MappingDims = unique( ['<%d,%d>' % (x.dim_ref, x.dim_phys-x.dim_ref)
                                    for x in self.table] )
        self.MappingDims = self.MappingDims + unique( ['<%d,%d>' % (x.dim_ref, 0)
                                    for x in self.table] )
        self.MappingDims = unique(self.MappingDims)
        self.UserMappingDims = unique( ['<%d,%d>' % (x.dim_ref, x.dim_phys-x.dim_ref)
                                        for x in self.user_table] )
        return None
    
    
    
    def create_PhysSpaces(self):
 
        self.PushForwards = unique(['PushForward<Transformation::h_grad, %d,%d>' 
                                    %(x.dim_ref, x.dim_phys - x.dim_ref) for x in self.table] )
        self.PushForwards = self.PushForwards + \
            (unique(['PushForward<Transformation::h_grad, %d,%d>' 
                     %(x.dim_ref, 0) for x in self.table] ) )
        self.PushForwards = unique(self.PushForwards)
               
        spaces =('BSplineSpace', 'NURBSSpace')
        self.PhysSpaces = unique( ['PhysicalSpace <' +
                                   '%s<%d,%d,%d>' % (sp, x.dim_ref, x.range_ref, x.rank_ref) +
                                   ', PushForward<Transformation::h_grad, %d,%d> >' 
                                   % (x.dim_ref, x.dim_phys-x.dim_ref) 
                                   for sp in spaces
                                   for x in self.table] )
        
        self.UserPhysSpaces = \
            unique( ['PhysicalSpace <' +
                     '%s<%d,%d,%d>' % (sp, x.dim_ref, x.range_ref, x.rank_ref) +
                     ', PushForward<Transformation::h_grad, %d,%d> >' 
                     % (x.dim_ref, x.dim_phys-x.dim_ref) 
                    for sp in spaces
                    for x in self.user_table] )
            
           
    def create_ref_dim(self):
        self.ref_dom_dims = unique([x.dim_ref for x in self.table])
        self.user_ref_dom_dims = unique([x.dim_ref for x in self.user_table])
        self.face_ref_dom_dims = unique([x.dim_ref for x in self.face_table])
        return None

    
    def create_derivatives(self):
        '''Creates a list of the tensor types for the required values and derivatives'''
        C_list=[]
        #Values and derivatives for the physical spaces
        for row in self.table:
            for order in self.deriv_order:
                dim_domain = row.dim_ref
                dim_range  = row.range_ref
                rank  = row.rank_ref
                if order==0:
                    first_part = 'Tensor< %d, %d, tensor::covariant,' %(dim_domain, 1)
                else:
                    first_part = 'Tensor< %d, %d, tensor::covariant,' %(dim_domain, order)
                second_part = 'Tensor< %d, %d, tensor::contravariant, Tdouble> >' %(dim_range, rank)
                    
                C_list.append (first_part + second_part)
                
                
                dim_domain = row.dim_phys
                dim_range  = row.range_ref
                rank  = row.rank_ref
                if order==0:
                    first_part = 'Tensor< %d, %d, tensor::covariant,' %(dim_domain, 1)
                else:
                    first_part = 'Tensor< %d, %d, tensor::covariant,' %(dim_domain, order)
                    second_part = 'Tensor< %d, %d, tensor::contravariant, Tdouble> >' %(dim_range, rank)
                    
                C_list.append (first_part + second_part)
                
                #for the mapping
                dim_domain = row.dim_ref
                dim_range  = row.dim_phys
                rank  = 1
                if order==0:
                    first_part = 'Tensor< %d, %d, tensor::covariant,' %(dim_domain, 1)
                else:
                    first_part = 'Tensor< %d, %d, tensor::covariant,' %(dim_domain, order)
                    second_part = 'Tensor< %d, %d, tensor::contravariant, Tdouble> >' %(dim_range, rank)
                    
                C_list.append (first_part + second_part)
                       
                dim_domain = row.dim_phys
                dim_range  = row.dim_ref
                rank  = 1
                if order==0:
                    first_part = 'Tensor< %d, %d, tensor::covariant,' %(dim_domain, 1)
                else:
                    first_part = 'Tensor< %d, %d, tensor::covariant,' %(dim_domain, order)
                    second_part = 'Tensor< %d, %d, tensor::contravariant, Tdouble> >' %(dim_range, rank)
                    
                C_list.append (first_part + second_part)
                       
                dim_domain = row.dim_ref
                dim_range  = row.dim_phys
                rank  = 1
                if order==0:
                    first_part = 'Tensor< %d, %d, tensor::covariant,' %(dim_domain, 1)
                else:
                    first_part = 'Tensor< %d, %d, tensor::covariant,' %(dim_domain, order)
                    second_part = 'Tensor< %d, %d, tensor::contravariant, Tdouble> >' %(dim_range, rank)
                    
                C_list.append (first_part + second_part)
                       
                dim_domain = row.dim_ref
                dim_range  = row.dim_ref
                rank  = 1
                if order==0:
                    first_part = 'Tensor< %d, %d, tensor::covariant,' %(dim_domain, 1)
                else:
                    first_part = 'Tensor< %d, %d, tensor::covariant,' %(dim_domain, order)
                    second_part = 'Tensor< %d, %d, tensor::contravariant, Tdouble> >' %(dim_range, rank)
                    
                C_list.append (first_part + second_part)
                       
                self.derivatives= unique(C_list)
    
        return None


def intialize_instantiation():
    """
    Main function called at the beginning of all instatiation scripts.
    """

    # Getting a dictionary or arguments.
    from sys import argv as sysargv
    from os import sep as ossep
    args = dict([arg.split('=') for arg in sysargv[1:]])
    
    # Reading information from dimensions file.
    inst = InstantiationInfo(args['config_file'], args['max_der_order'])
    
#   Some debug information printing
    if False:  
        for x in inst.table:    
            print (x.dim_ref, x.range_ref, x.rank_ref, x.dim_phys) 
#    print inst.deriv_order
    
    # Openning the output file.
    file_output = open(args['out_file'], 'w')
    # Writing the header.
    header = ( '// This file was automatically generated' +
               'from %s \n' % (sysargv[0].split(ossep)[-1]) +
               '// DO NOT edit as it will be overwritten.\n\n')
    file_output.write(header)


    return file_output, inst
